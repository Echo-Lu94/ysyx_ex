module barrel_shifter #(
    DWIDTH=16,
    SHIFT_NUM=4
)(
    input clk,
    input [DWIDTH-1:0]data_i,
    input [SHIFT_NUM-1:0]shamt,
    input L_R,
    input A_L,
    output [DWIDTH-1:0]data_o
);
reg [DWIDTH-1:0]data_w;

assign data_o =  data_w;

always@(*)
        //A_L,1:arith; 0:logic
        //L_R,1:left; 0:right
        case({A_L,L_R})
            2'b00:begin//逻辑右移
                data_w = shamt[0] ? {1'b0, data_i[DWIDTH-1:1]} : data_i;
                data_w = shamt[1] ? {{2{1'b0}}, data_w[DWIDTH-1:2]} : data_w;
                data_w = shamt[2] ? {{4{1'b0}}, data_w[DWIDTH-1:4]} : data_w;
                data_w = shamt[3] ? {{8{1'b0}}, data_w[DWIDTH-1:8]} : data_w;
            end
            2'b01,2'b11:begin//左移，算术同逻辑
                data_w = shamt[0] ? {data_i[DWIDTH-2:0],1'b0} : data_i;
                data_w = shamt[1] ? {data_w[DWIDTH-3:0],{2{1'b0}}} : data_w;
                data_w = shamt[2] ? {data_w[DWIDTH-5:0],{4{1'b0}}} : data_w;
                data_w = shamt[3] ? {data_w[DWIDTH-9:0],{8{1'b0}}} : data_w;
            end
            2'b10:begin//算术右移
                data_w = shamt[0] ? {data_i[DWIDTH-1],data_i[DWIDTH-1:1]} : data_i;
                data_w = shamt[1] ? {{2{data_w[DWIDTH-1]}},data_w[DWIDTH-1:2]} : data_w;
                data_w = shamt[2] ? {{4{data_w[DWIDTH-1]}},data_w[DWIDTH-1:4]} : data_w;
                data_w = shamt[3] ? {{8{data_w[DWIDTH-1]}},data_w[DWIDTH-1:8]} : data_w;
            end
            default:data_w = data_i;
        endcase

        endmodule
