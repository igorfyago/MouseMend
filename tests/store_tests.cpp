// store_tests.cpp : how the Tuner's wheel click test counts, fed with made-up
// events. Nothing here touches the mouse. The store writes its files to a
// folder under %TEMP%.
//
//   build.ps1 builds and runs it with the rule tests.

#include <cstdio>
#include <cstdlib>
#include <string>

#include "store.h"

using namespace fbw;

static int checks = 0, failed = 0;

static void check(bool ok, const char* what) {
    ++checks;
    if (!ok) {
        ++failed;
        printf("FAIL  %s\n", what);
    }
}

// One button event from the guarded mouse. blocked: the guard dropped it.
static Rec button(double t, int b, bool down, bool blocked = false, double sys = -1) {
    Rec r;
    r.t = t;
    r.sys = static_cast<uint32_t>(sys >= 0 ? sys : t);   // Windows' coarse clock, unless given: agrees with the fine one
    r.lag = 0;
    r.kind = kButton;
    r.button = static_cast<uint8_t>(b);
    r.down = down ? 1 : 0;
    r.source = kHardware;
    r.blocked = blocked ? 1 : 0;
    r.verdict = blocked ? static_cast<uint8_t>(down ? Verdict::BouncePress : Verdict::BounceRelease) : 0;
    r.device = 1;
    r.judged = 1;
    return r;
}

static std::wstring scratchDir() {
    wchar_t* temp = nullptr;
    size_t n = 0;
    std::wstring dir = (_wdupenv_s(&temp, &n, L"TEMP") == 0 && temp) ? std::wstring(temp) : std::wstring(L".");
    free(temp);
    dir += L"\\mousemend-store-tests";
    CreateDirectoryW(dir.c_str(), nullptr);
    return dir;
}

int main() {
    const std::wstring dir = scratchDir();

    // one press at a time; a second press soon after letting go is the switch
    {
        Store s;
        s.open(dir);
        check(s.startTest(kTestWheel, 50, true, 0), "a wheel click test starts");
        s.add(button(400, Middle, true));             // by hand
        s.add(button(480, Middle, false));
        s.add(button(505, Middle, true, true));       // 25 ms after letting go: the switch, dropped by the guard
        s.add(button(515, Middle, false, true));
        s.add(button(1000, Middle, true));            // by hand
        s.add(button(1070, Middle, false));
        s.add(button(1100, Middle, true));            // 30 ms after letting go: the switch, and it got through
        s.add(button(1110, Middle, false));
        s.add(button(1500, Left, true));              // other buttons do not count, double or not
        s.add(button(1520, Left, false));
        s.add(button(1540, Left, true));
        s.add(button(1600, Left, false));
        s.add(button(2000, Middle, true));            // by hand
        s.add(button(2080, Middle, false));
        const TestRun r = s.currentRun();
        check(r.id == kTestWheel && r.active, "the run is the wheel click test");
        check(r.good == 3, "three presses by hand");
        check(r.caught == 1, "one double press fixed by the guard");
        check(r.wrong == 1, "one double press that got through");
        check(r.eaten == 0, "no press by hand dropped");
        check(r.wrongGaps.size() == 1 && r.wrongGaps[0] == 30.0f, "the one that got through came 30 ms after letting go");
        s.stopTest(5000);
    }
    // the line between the switch and the hand
    {
        Store s;
        s.open(dir);
        s.startTest(kTestWheel, 50, true, 0);
        s.add(button(400, Middle, true));
        s.add(button(450, Middle, false));
        s.add(button(599, Middle, true));             // 149 ms after letting go: a double press
        s.add(button(610, Middle, false));
        s.add(button(760, Middle, true));             // 150 ms after letting go: a press by hand
        s.add(button(800, Middle, false));
        const TestRun r = s.currentRun();
        check(r.wrong == 1 && r.good == 2, "at the default 150 ms: 149 ms after letting go is a double press, 150 ms a press by hand");
        s.stopTest(5000);
    }
    // no lead for the wheel test: the click on Start is never a wheel press
    {
        Store s;
        s.open(dir);
        s.startTest(kTestWheel, 50, true, 0);
        s.add(button(100, Middle, true));
        s.add(button(150, Middle, false));
        const TestRun r = s.currentRun();
        check(r.good == 1 && r.wrong == 0 && r.caught == 0, "a wheel press right after Start counts");
        s.stopTest(5000);
    }
    // two late callbacks make a hand's press look quick on the fine clock: Windows' stamps tell
    {
        Store s;
        s.open(dir);
        s.startTest(kTestWheel, 50, true, 0);
        s.add(button(400, Middle, true));
        s.add(button(450, Middle, false, false, 450));
        s.add(button(520, Middle, true, false, 620));  // 70 ms on the fine clock, 170 on Windows' own: the hand
        s.add(button(600, Middle, false, false, 700));
        const TestRun r = s.currentRun();
        check(r.good == 2 && r.wrong == 0, "a press Windows stamped 170 ms after letting go is the hand's, whatever the fine clock says");
        s.stopTest(5000);
    }
    // the clicks test leaves the wheel button to its own test
    {
        Store s;
        s.open(dir);
        s.startTest(kTestClicks, 100, true, 0);
        s.add(button(400, Middle, true));              // a wheel press and its bounce: not part of the clicks test
        s.add(button(470, Middle, false));
        s.add(button(495, Middle, true, true));
        s.add(button(525, Middle, false, true));
        s.add(button(1000, Left, true));               // a left click: counted
        s.add(button(1090, Left, false));
        const TestRun r = s.currentRun();
        check(r.good == 1 && r.wrong == 0 && r.caught == 0 && r.eaten == 0, "the clicks test counts the left click and nothing of the wheel");
        s.stopTest(5000);
    }
    // the line is the wheel's own re-press window: change the setting and the test follows
    {
        Store s;
        s.open(dir);
        AppConfig c;
        c.rules.bounceMs[Middle] = 60;
        s.noteConfig(c, 0);
        s.startTest(kTestWheel, 50, true, 0);
        s.add(button(400, Middle, true));
        s.add(button(450, Middle, false));
        s.add(button(500, Middle, true));             // 50 ms: inside a 60 ms window
        s.add(button(510, Middle, false));
        s.add(button(580, Middle, true));             // 70 ms: outside it
        s.add(button(620, Middle, false));
        const TestRun r = s.currentRun();
        check(r.wrong == 1 && r.good == 2, "with the wheel set to 60 ms the test's line is 60 ms too");
        s.stopTest(5000);
    }
    // the test has a name the page and the saved runs use
    check(std::string(testName(kTestWheel)) == "wheel", "the wheel click test is called wheel");

    printf("%d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}
