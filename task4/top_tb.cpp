#include "Vtop.h"
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
    Vtop* top = new Vtop;
    // init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("top.vcd");

    // init Vbuddy
    if (vbdOpen()!=1) return(-1);
    vbdHeader("Lab 1: Top");

    // initialize simulation inputs
    top->clk = 1;
    top->rst = 1;
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

        top->en = vbdFlag();

        vbdHex(4, (int(top->bcd) >> 12) & 0xF);
        vbdHex(3, (int(top->bcd) >> 8) & 0xF);
        vbdHex(2, (int(top->bcd) >> 4) & 0xF);
        vbdHex(1, int(top->bcd) & 0xF);

        // This checks whether the simulation has been signaled to stop, which would cause an early exit.
        if (Verilated::gotFinish()) exit(0);

    }
    // This finalize the waveform for GTKWave, and exit(0) exits the program.

    vbdClose();
    tfp->close();
    exit(0);
}
