# 3DS CPU Tests

An application I use to verify hardware behavior in citra.
This mostly cobbles Valgrind's hardware tests into a 3DS application that can be run through Homebrew Browser.

#### How To Build It

1. Install the latest devkitARM.
2. Clone the project.
3. Run `make` inside the directory.
4. Go make your favorite tea or coffee, because it takes a bit to build.

#### How To Use It

1. Run it on your 3DS.
2. Select the tests you want to run.
3. Run the same tests on citra.
4. Diff the two output files with one another (preferably with colordiff or something that doesn't output in monocolor).
5. If the difference for the citra-generated file differs from the 3DS one. Then your emulation is innaccurate.


#### Notes

- In the VFP test file, at the beginning of `VFPMain`, there is some ifdef'd out assembly.
  - This assembly enables RunFast mode for the VFP, which can be useful to test against
    as some games (Cubic Ninja for example) will enable it, since perfect IEEE compliance
    is not always necessary.