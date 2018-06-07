# Organisation

**OpenSolution.bat**: open the vs solution containing all 4 projects.

**BranchMemorize**: code project of the Branch and Memorize

**sips**: code project of Tanaka's algorithm

**Tester1Dtilde**: code project of the tester. (the VS solution file is inside)

**Output**: the exe file of all projects are copied to this folder. Use this folder to run any program. The dataset is also there. **Extract the data first**.

# Parameters

### To run the tester:

Put parameters in **tester_config.txt**, then run **tester.exe  tester_config.txt**

The meaning of parameters can be found in **tester_config.txt**. 

Or in **Tester1Dtilde/testutils.h**. Check the **Config** class.

The data **.zip**  in **Output** folder should be decompressed first.

Also check the macro parameters in *testexact1.cpp*. (PMAX, WMAX)

### To run the BB (pvw.exe):

- The datafile path should be passed as argument, otherwise use **donnees.dat** by default.

- the **pvw.ini** must exist. One parameter per line

- Line 1: Search Strategy

   0, classical depth first
   1, classical best first
   2, classical breadth first
   3, depth first memo
   4, best first memo
   5, breadth first memo
   6, solution/lb memo (depth first)

- Line 2: LB Strategy. always use **0**.

- Line 3: Clean Strategy. Put **3** to use LUFO.

- Line 4: Dominance Strategy. Put **0** to disable or **12** to enable predictive dominance (by checking k-permutation)


### To run the sips program:

- arguments: "sips.exe datafile nbJobs"
- Check the macros in *extension.h* and *sips_common.h* (e.x. P_MAX, etc)