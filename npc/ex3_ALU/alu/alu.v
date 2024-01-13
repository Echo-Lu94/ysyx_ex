module alu #(
    DWIDTH = 4
)(
`ifdef NVBOARD
    input clk,
`endif
    input [DWIDTH-1 :0]a,
    input [DWIDTH-1 :0]b,
    input [2:0]fun_sel,
//as the ex says, there's no carry in signed data cal
    output carry,
    output zero,
    output overflow,
    output comp_o,
    output [DWIDTH-1 :0]result
);

assign zero = ~(| result_w);
assign carry = carry_w;
assign overflow = overflow_w;
assign result = result_w;
assign comp_o = comp_w;

reg carry_w;
reg overflow_w;
reg [DWIDTH-1:0]result_w;
reg [DWIDTH-1:0]t_add_cin;
//wire [DWIDTH:0]t_add_cin_w;
reg comp_w;
//wire [DWIDTH:0]a_w;
//wire [DWIDTH:0]b_w;
//assign a_w = {a[DWIDTH-1],a};
//assign b_w = {b[DWIDTH-1],b};
//assign t_add_cin_w = {t_add_cin[DWIDTH-1],t_add_cin};

always@(*) begin
    carry_w = 0;
    overflow_w = 0;
    result_w = 'd0;
    comp_w = 0;
    t_add_cin = 0;
    casez(fun_sel)
        3'b00?:begin //add or sub
        /* verilator lint_off WIDTHEXPAND */
            t_add_cin = ({DWIDTH{fun_sel[0]}}^b) + fun_sel[0];
            {carry_w, result_w} = a + t_add_cin;
//            result_w = a + t_add_cin;
            if(fun_sel[0]&&b[DWIDTH-1]&&(b[DWIDTH-2:0]=='d0))//减法时，减数为最小值
                overflow_w = 0;
            else
                overflow_w = (a[DWIDTH-1] == t_add_cin[DWIDTH-1]) && (result_w[DWIDTH-1] != a[DWIDTH-1]);
        end
        3'b010: //not
            result_w = ~a;
        3'b011: //and
            result_w = a&b;
        3'b100: //or
            result_w = a|b;
        3'b101: //xor
            result_w = a^b;
        3'b110:begin //compare
        /* verilator lint_off WIDTHEXPAND */
            t_add_cin = ({DWIDTH{1'b1}}^b) + 1;
            result_w = a + t_add_cin;
            if(b[DWIDTH-1]&&(b[DWIDTH-2:0]=='d0))//减数为最小值 
                overflow_w = 0;
            else
                overflow_w = (a[DWIDTH-1] == t_add_cin[DWIDTH-1]) && (result_w[DWIDTH-1] != a[DWIDTH-1]);
            comp_w = result_w[DWIDTH-1]^overflow_w;
        end
        3'b111:begin//eq
            /* verilator lint_off WIDTHEXPAND */
            result_w = a + ({DWIDTH{1'b1}}^b) + 1;
            comp_w = ~(|result_w);
        end

    endcase
end

endmodule
