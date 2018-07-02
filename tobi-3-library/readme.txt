README
Andrea Frank
7/10/2017

Original library and header file created by Cherag Bhagwagar (10/21/2016). Updates (denoted by $ at
the start of comments) and re-organization by Andrea Frank (7/10/2017). 


***** CHANGELOG: *****

By: Andrea Frank, 7/10/17	***********************************************************************
Major changes: Set __pwmPin[1] to 12 instead of 5, since __encoderPin[5] = 5 a few lines down. I am
under the impression that this pin should not be written to __pwmPin[1] and then immediately after
also assigned as an input pin for __encoderPin[5], but I will check with Cherag.

Minor changes: Reorganization, commenting code.
***************************************************************************************************