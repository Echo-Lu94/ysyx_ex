module light(
  input clk,
  input rst,
  output reg [15:0] led
);
//    _7_
//  2|   |6
//   |_1_|
//  3|   |5 
//   |_4_|


  reg [31:0] count;
  always @(posedge clk) begin
    if (rst) begin led <= 1; count <= 0; end
    else begin
      if (count == 0) led <= {led[14:0], led[15]};
      count <= (count >= 5000000 ? 32'b0 : count + 1);
    end
  end

  
endmodule
