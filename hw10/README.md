### Overview of Filters

#### FIR
The FIR Filter implemented is a 10th-order filter with a cutoff frequency of 5Hz, chosen in Matlab using
      
      fir1(9,.05)
The code implementation is in the function [fir](https://github.com/tehwentzel/ME-433/blob/9dfde4aa4c0a8c99630d5e06cfce892b49063022/hw10/firmware/src/app.c#L300) in app.c, which uses a rolling history and an array of hardcoded coefficients initialized with the function [init_firvals()](https://github.com/tehwentzel/ME-433/blob/9dfde4aa4c0a8c99630d5e06cfce892b49063022/hw10/firmware/src/app.c#L281).

#### MIR 
The MIR Filter was made to be 10th-order to match the FIR filter, and is implemented in the [mir()](https://github.com/tehwentzel/ME-433/blob/9dfde4aa4c0a8c99630d5e06cfce892b49063022/hw10/firmware/src/app.c#L312) function in app.c

#### IRR
THe IRR filter is calculated in a single line [here](https://github.com/tehwentzel/ME-433/blob/9dfde4aa4c0a8c99630d5e06cfce892b49063022/hw10/firmware/src/app.c#L535), where the initial value is set to zero for the base-case.
[Alpha, Beta, and Scale](https://github.com/tehwentzel/ME-433/blob/9dfde4aa4c0a8c99630d5e06cfce892b49063022/hw10/firmware/src/app.c#L66) are configurable constants that define the proportion of the original value stays during the new value.  For this case the constants are chosen so the mean of previous value of the IRR filter and the current Accelerometer reading is used.


The effect of each filter on a hand-input impulse can be seen here: 
![here](https://github.com/tehwentzel/ME-433/blob/master/hw10/PIC_FILTER_RESPONSE_plot.png)
All filters are effective at reducing noise over the original data.  However, the MIR filter appears to lose a good amount of information due to the smooting effects, while the FIR filter doesn't.  The FIR Filter takes about 30 extra operations to complete since a rolling buffer is used: one addition, one multiplication, and one modulo operator for each character in the coefficient array for the indexing, making them both the same order of complexity ~O(N).  The extra opperations seem reasonable for the gain in data for all but extreme cases, where the IRR would be the most appropriate.  
