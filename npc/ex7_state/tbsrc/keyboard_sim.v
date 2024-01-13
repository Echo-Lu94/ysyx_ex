module keyboard_sim;

/* parameter */
parameter [31:0] clock_period = 10;

/* ps2_keyboard interface signals */
reg clk,rst_n;
/* verilator lint_off UNUSEDSIGNAL */
wire [7:0] data;
wire ready,overflow;
wire kbd_clk, kbd_data;
reg nextdata_n;
wire [7:0]seg0,seg1,seg2,seg3;

ps2_keyboard_model model(
    .ps2_clk(kbd_clk),
    .ps2_data(kbd_data)
);

ps2_keyboard inst(
    .clk(clk),
    .rst_n(rst_n),
    .ps2_clk(kbd_clk),
    .ps2_data(kbd_data),
    .data(data),
    .ready(ready),
    .nextdata_n(nextdata_n),
    .overflow(overflow),
    .seg0(seg0),
    .seg1(seg1),
    .seg2(seg2),
    .seg3(seg3)
);

initial begin /* clock driver */
    clk = 0;
    forever
        #(clock_period/2) clk = ~clk;
end

initial begin
    nextdata_n =1'b1;
    rst_n = 1'b0;  #20;
    rst_n = 1'b1;  #20;
    model.kbd_sendcode(8'h1C); // press 'A'
//    #50 nextdata_n =1'b0; #50 nextdata_n =1'b1;//read data
    model.kbd_sendcode(8'hF0); // break code
//    #50 nextdata_n =1'b0; #50 nextdata_n =1'b1; //read data
    model.kbd_sendcode(8'h1C); // release 'A'
//    #50 nextdata_n =1'b0; #50 nextdata_n =1'b1; //read data
    model.kbd_sendcode(8'h1B); // press 'S'
    #50 model.kbd_sendcode(8'h1B); // keep pressing 'S'
    #50 model.kbd_sendcode(8'h1B); // keep pressing 'S'
    model.kbd_sendcode(8'hF0); // break code
    model.kbd_sendcode(8'h1B); // release 'S'
    #50 nextdata_n=0;
    #20;
    $display("finished!");
//    $stop;
end

endmodule
