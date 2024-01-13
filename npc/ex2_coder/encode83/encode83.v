module encode83(
`ifdef NVBOARD
    input clk,
`endif
    input [7:0]encode83_i,
    input en,
    output indicator,
    output [2:0]encode83_o,
    output [7:0]seg0_o
//    output [7:0]bin2gray_o,
//    output [7:0]gray2bin_o
);

reg [2:0] encode83_ow;

assign indicator = |encode83_i;
assign encode83_o = encode83_ow;
//assign bin2gray_o = {encode83_i>>1)^encode83_i;
//
//integer i;
//always@(*) begin
//    if(en) begin
//        encode83_ow = 'd0;
//        for(i=0; i<8; i=i+1)
//            if(encode83_i[i]==1) encode83_ow = i[2:0];
//    end
//    else encode83_ow = 'd0;
//end

always@(*) begin
    if(en) begin
        casez(encode83_i)
            8'b1???????: encode83_ow = 3'd7;
            8'b01??????: encode83_ow = 3'd6;
            8'b001?????: encode83_ow = 3'd5;
            8'b0001????: encode83_ow = 3'd4;
            8'b00001???: encode83_ow = 3'd3;
            8'b000001??: encode83_ow = 3'd2;
            8'b0000001?: encode83_ow = 3'd1;
            8'b00000000: encode83_ow = 3'd0;
            default:     encode83_ow = 3'd0;
        endcase
    end
end

bcd7seg seg0( {1'b0,encode83_o},seg0_o);

endmodule

