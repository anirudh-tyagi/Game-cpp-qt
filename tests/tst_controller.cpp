// Covers the boundary arithmetic in Controller, which is the part of this
// codebase that is genuinely hard to confirm by eye: what a resize does to the
// ship, where the ceiling and floor actually stop it, and where enemies may
// spawn. game_core has no Quick dependency, so all of it runs headless.
//
// The controller's timers are started but never allowed to fire: the event loop
// never spins here, and updateState() is driven by hand. That keeps every test
// deterministic rather than dependent on wall-clock timing.

#include <QSignalSpy>
#include <QTest>

#include "core/Controller.h"
#include "core/Enemy.h"
#include "core/GameConfig.h"

using namespace GameConfig;

namespace {
//the window the tests use, and the values Controller derives from it
constexpr double testWidth = 800.0;
constexpr double testHeight = 600.0;
constexpr double testMaxX = testWidth - playerWidth;    //750
constexpr double testBottomY = testHeight - playerHeight; //550

double ceilingFor(double bottomY){
    return bottomY * ceilingFraction;
}

int enemyCount(Controller& control){
    QQmlListProperty<Enemy> list = control.enemies();
    return static_cast<int>(list.count(&list));
}

Enemy* enemyAt(Controller& control, int index){
    QQmlListProperty<Enemy> list = control.enemies();
    return list.at(&list, index);
}
}

class TestController: public QObject
{
    Q_OBJECT

private slots:
    void levelFollowsScore();
    void levelFollowsScore_data();
    void startGameResetsTheRun();

    void resizeKeepsAltitudeWhileRunning();
    void resizeParksTheShipWhileStopped();
    void resizePullsTheShipInsideANarrowerWindow();

    void shipStopsAtTheCeiling();
    void shipRestsOnTheFloor();

    void enemiesSpawnFullyOnScreen();

    void pauseHoldsAndReleasesTheRun();
    void pauseIgnoresPlayInput();
};

// ---------------------------------------------------------------- score/level

void TestController::levelFollowsScore_data()
{
    QTest::addColumn<double>("score");
    QTest::addColumn<int>("level");

    QTest::newRow("fresh") << 0.0 << 1;
    QTest::newRow("just under the first threshold") << pointsPerLevel - 1 << 1;
    QTest::newRow("exactly on it") << pointsPerLevel << 2;
    QTest::newRow("two and a half levels in") << pointsPerLevel * 2.5 << 3;
}

void TestController::levelFollowsScore()
{
    QFETCH(double, score);
    QFETCH(int, level);

    Controller control;
    control.setScore(score);

    QCOMPARE(control.level(), level);
}

void TestController::startGameResetsTheRun()
{
    Controller control;
    control.setBoundaries(testWidth, testHeight);

    control.startGame();
    control.setScore(pointsPerLevel * 4);
    QCOMPARE(control.level(), 5);

    control.startGame();

    QCOMPARE(control.score(), 0.0);
    QCOMPARE(control.level(), 1);
    QVERIFY(control.running());
    QVERIFY(!control.gameOver());
    QVERIFY(!control.paused());
    //centred, and the left edge is what x refers to
    QCOMPARE(control.x(), testMaxX / 2);
    QCOMPARE(control.y(), testBottomY);
}

// -------------------------------------------------------------------- resizes

void TestController::resizeKeepsAltitudeWhileRunning()
{
    Controller control;
    control.setBoundaries(testWidth, testHeight);
    control.startGame();

    //climb a little
    control.applyThrust();
    control.updateState();
    control.updateState();

    const double altitude = control.y();
    QVERIFY(altitude < testBottomY);

    //a resize that does not change the height must not move the ship at all.
    //this is the regression: setBoundaries used to slam y back to the floor
    control.setBoundaries(1200, testHeight);

    QCOMPARE(control.y(), altitude);
}

void TestController::resizeParksTheShipWhileStopped()
{
    Controller control;

    //no run in progress, so the ship belongs on the floor, ready for the next one
    control.setBoundaries(testWidth, testHeight);
    QCOMPARE(control.y(), testBottomY);

    control.setBoundaries(testWidth, 400);
    QCOMPARE(control.y(), 400 - playerHeight);
}

void TestController::resizePullsTheShipInsideANarrowerWindow()
{
    Controller control;
    control.setBoundaries(testWidth, testHeight);
    control.startGame();
    QCOMPARE(control.x(), testMaxX / 2); //375

    //the window shrinks out from under the ship
    control.setBoundaries(200, testHeight);
    QCOMPARE(control.x(), 200 - playerWidth); //150, not left stranded at 375

    //and a shorter window brings it down to the new floor
    control.setBoundaries(200, 300);
    QCOMPARE(control.y(), 300 - playerHeight);
}

// ------------------------------------------------------------------- the walls

void TestController::shipStopsAtTheCeiling()
{
    Controller control;
    control.setBoundaries(testWidth, testHeight);
    control.startGame();

    //thrust held down for far longer than it takes to reach the ceiling.
    //the old code tested altitude on the key press only, so thrust applied just
    //below the line carried the ship straight through it
    for(int frame = 0; frame < 300; frame++){
        control.applyThrust();
        control.updateState();
        QVERIFY2(control.y() >= ceilingFor(testBottomY) - 1e-9,
                 qPrintable(QStringLiteral("climbed past the ceiling to y=%1")
                                .arg(control.y())));
    }

    QVERIFY(qFuzzyCompare(control.y(), ceilingFor(testBottomY)));

    //and it falls back once thrust stops, rather than sticking to the ceiling.
    //two frames, not one: position is integrated with the previous frame's
    //velocity before gravity is added, so the frame straight after a clamp
    //still carries the zero the clamp left behind
    control.updateState();
    control.updateState();
    QVERIFY(control.y() > ceilingFor(testBottomY));
}

void TestController::shipRestsOnTheFloor()
{
    Controller control;
    control.setBoundaries(testWidth, testHeight);
    control.startGame();

    for(int frame = 0; frame < 300; frame++){
        control.updateState();
    }

    QCOMPARE(control.y(), testBottomY);
    QVERIFY(!control.thrusting());

    //a single thrust from rest lifts it by exactly one frame of thrust, which
    //it would not if ySpeed had been accumulating gravity the whole time
    control.applyThrust();
    control.updateState();
    QCOMPARE(control.y(), testBottomY + maxThrust);
}

// -------------------------------------------------------------------- spawning

void TestController::enemiesSpawnFullyOnScreen()
{
    Controller control;
    control.setBoundaries(testWidth, testHeight);
    control.startGame();

    for(int i = 0; i < 400; i++){
        control.createEnemies();
    }

    const int count = enemyCount(control);
    QVERIFY(count > 0);

    //the range is sized off enemyWidth, not the player's, so the whole sprite
    //lands on screen and the right edge stays reachable
    const double rightmost = testWidth - enemyWidth;
    for(int i = 0; i < count; i++){
        const double x = enemyAt(control, i)->x();
        QVERIFY2(x >= 0.0 && x <= rightmost,
                 qPrintable(QStringLiteral("enemy spawned at x=%1, outside [0, %2]")
                                .arg(x).arg(rightmost)));
    }
}

// ----------------------------------------------------------------------- pause

void TestController::pauseHoldsAndReleasesTheRun()
{
    Controller control;
    control.setBoundaries(testWidth, testHeight);

    //nothing to hold before a run starts
    control.togglePause();
    QVERIFY(!control.paused());

    control.startGame();
    QSignalSpy spy(&control, &Controller::pausedChanged);

    control.togglePause();
    QVERIFY(control.paused());
    QVERIFY(control.running()); //paused is a live run, not the absence of one
    QCOMPARE(spy.count(), 1);

    control.togglePause();
    QVERIFY(!control.paused());
    QCOMPARE(spy.count(), 2);

    //and a restart from a paused run comes back unpaused
    control.togglePause();
    control.startGame();
    QVERIFY(!control.paused());
    QVERIFY(control.running());
}

void TestController::pauseIgnoresPlayInput()
{
    Controller control;
    control.setBoundaries(testWidth, testHeight);
    control.startGame();
    control.togglePause();

    const double x = control.x();
    const double y = control.y();

    control.moveLeft();
    control.moveRight();
    control.applyThrust();
    control.fireBullet();
    control.createEnemies();

    QCOMPARE(control.x(), x);
    QCOMPARE(control.y(), y);
    QCOMPARE(enemyCount(control), 0);

    QQmlListProperty<Bullet> bullets = control.bullets();
    QCOMPARE(bullets.count(&bullets), 0);
}

QTEST_GUILESS_MAIN(TestController)

#include "tst_controller.moc"
