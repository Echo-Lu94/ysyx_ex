module shift(
    input clk,
    input rst_n,
    input data_i,
    input [2:0]ctrl,
//    input ps2_clk,
//    input ps2_data,
    output [7:0]data_o
);
reg [7:0]data_r;
//reg [31:0]count;

assign data_o =  data_r;
always@(posedge clk or negedge rst_n)
    if(!rst_n)begin
        data_r <= 'd0;
//        count <= 'd0;
    end
    else begin
//            count <= (count >= 30000000 ? 32'b0 : count + 1);
        case(ctrl)
            3'b000:begin//clear
//                if(count==0) 
                    data_r <= 'd0;
 //               count <= (count >= 30000000 ? 32'b0 : count + 1);
            end

            3'b001:begin//set 1
//                if(count==0)    
                    data_r <= 'd1;
 //               count <= (count >= 30000000 ? 32'b0 : count + 1);
            end
            3'b010:begin//right logic shift
//                if(count==0)
                    data_r <= {1'b0,data_r[7:1]};
 //               count <= (count >= 10000000 ? 32'b0 : count + 1);
            end
            3'b011:begin//left logic shift
//                if(count==0)
                    data_r <= {data_r[6:0],1'b0};
//                count <= (count >=10000000 ? 32'b0 : count + 1);
            end
            3'b100:begin//arith right shift
//                if(count==0)
                    data_r <= {data_r[7],data_r[7:1]};
//                count <= (count >=10000000 ? 32'b0 : count + 1);
            end
            3'b101:begin//data in
//                if(count==0)
                    data_r <= {data_i,data_r[7:1]};
//                count <= (count >=10000000 ? 32'b0 : count + 1);
            end
            3'b110:begin//cycle right shift
//                if(count==0)
                    data_r <= {data_r[0],data_r[7:1]};
//                count <= (count >=10000000 ? 32'b0 : count + 1);
            end
            3'b111:begin//cycle left shift
//                if(count==0)
                    data_r <= {data_r[6:0],data_r[7]};
//                count <= (count >=10000000 ? 32'b0 : count + 1);
            end

            endcase
        end

        endmodule
