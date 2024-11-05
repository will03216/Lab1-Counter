#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

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

    // init Vbuddy
    if (vbdOpen()!=1) return(-1);
    vbdHeader("Lab 1: Counter");

    vbdSetMode(1);

    // initialize simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->ld = 0;
    top->v = 0;

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

        vbdCycle(i+1);

        top->rst = (i <2);

        top->ld = vbdFlag();

        vbdHex(4, (int(top->count) >> 12) & 0xF);
        vbdHex(3, (int(top->count) >> 8) & 0xF);
        vbdHex(2, (int(top->count) >> 4) & 0xF);
        vbdHex(1, int(top->count) & 0xF);

        // This checks whether the simulation has been signaled to stop, which would cause an early exit.
        if (Verilated::gotFinish()) exit(0);

    }
    // This finalize the waveform for GTKWave, and exit(0) exits the program.

    vbdClose();
    tfp->close();
    exit(0);
}