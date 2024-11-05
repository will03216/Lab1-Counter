#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char **argv, char **env) {
    int i;
    int clk;
    int pause_cycles = 0;
    bool paused = false;

    Verilated::commandArgs(argc, argv);
    // init top verilog instance
    Vcounter* top = new Vcounter;
    // init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("counter.vcd");

    // initialize simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->en = 0;

    // run simulation for many clock cycles
    for (i=0; i<300; i++) {

        // dump variables into VCD file and toggle clock

        // for each clock cycle, we toggle the clock signal twice, representing the rising and falling edge
        for (clk=0; clk<2; clk++){
            tfp->dump (2*i+clk); // using 2i + clk allows us to see the "within the clock cycle", representing accuractely the time ponit in the simulation
            top->clk = !top->clk; // This toggles the clock signal between high and low (simply negating itself), simulating the clock cycle.
            top->eval (); // the eval function tells Verilator to evaluate the current state in the device under test (DUT).
            // This means we are updating the internal logic of the counter based of the new input values.
        }

        top->rst = (i <2);

        if(top->count == 0x1A){
            top->rst = 1;
        }
        else if(top->count == 0x1B){
            top->rst = 0;
        }

        if(top->count == 0x9 && !paused){
            pause_cycles = 3;
            paused = true;
        }

        if(pause_cycles > 0 ){
            top->en = 0;
            pause_cycles--;
        }
        else if(paused && pause_cycles == 0){
            top->en = 1;
            paused = false;
        }
        else{
            top->en = (i > 4);
        }

        // This checks whether the simulation has been signaled to stop, which would cause an early exit.
        if (Verilated::gotFinish()) exit(0);

    }
    // This finalize the waveform for GTKWave, and exit(0) exits the program.

    tfp->close();
    exit(0);
}
