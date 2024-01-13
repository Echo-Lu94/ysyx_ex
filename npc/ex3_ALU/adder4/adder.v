module adder #(
    DWIDTH = 4
)(
    input [DWIDTH-1:0]a,
    input [DWIDTH-1:0]b,
    input fun_sel,//sub:1,add:0
    //as the ex said, there's no carry in signed data cal
    //在有符号的加减法中，溢出判断依据为溢出位，进位位不用
    output carry,
    output zero,
    output reg overflow,
    output [DWIDTH-1:0]result
);

wire [DWIDTH-1:0]t_add_cin;
/* verilator lint_off WIDTHEXPAND */
assign t_add_cin = ({DWIDTH{fun_sel}}^b ) + fun_sel;
assign {carry, result} = a + t_add_cin;
//assign  result = a + t_add_cin;

always@(*)
    if(fun_sel&&b[DWIDTH-1]&&(b[DWIDTH-2:0]=='d0))//减法时，减数为最小值
        overflow = 0;
    else
        overflow = (a[DWIDTH-1] == t_add_cin[DWIDTH-1]) && (result[DWIDTH-1] != a[DWIDTH-1]);
assign zero = ~ (|result);

endmodule
