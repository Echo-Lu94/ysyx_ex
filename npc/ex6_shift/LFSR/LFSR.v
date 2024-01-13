module LFSR(
    input clk,
    input rst_n,
    output [15:0]data_o
);
reg [7:0]data_r;
reg [31:0]count;

always@(posedge clk or negedge rst_n)
    if(!rst_n)begin
        data_r <= 'd1;
        count <= 'd0;
    end
    else begin
        count <= (count >= 10000000 ? 32'b0 : count + 1);
        if(count==0)begin
        data_r <= {data_r[4]^data_r[3]^data_r[2]^data_r[0],data_r[7:1]};

    end
    end

bcd7seg u0_bcd7seg(
    .b(data_r[3:0]),
    .h(data_o[7:0])
);  

bcd7seg u1_bcd7seg(
    .b(data_r[7:4]),
    .h(data_o[15:8])
);
        endmodule
