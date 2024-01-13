module ps2_keyboard(
    input clk,
    input rst_n,
    input ps2_clk,
    input ps2_data,
    input nextdata_n,
    output [7:0]data,
    output reg ready,
//    output sampling,
    output reg overflow,//fifo overflow
    output [7:0]seg0,
    output [7:0]seg1,
    output [7:0]seg2,
    output [7:0]seg3,
    output [7:0]seg4,
    output [7:0]seg5
);

//internal signal,for test
reg [9:0]buffer;//ps2_data bits
reg [7:0]fifo[7:0];//data_fifo
reg [2:0]w_ptr,r_ptr;//fifo write and read pointers
reg [3:0]count;//count ps2_data bits
//detect falling edge of ps2_clk
reg [2:0]ps2_clk_sync;
reg [31:0]cnt;//for nvboard
wire [7:0]asic;
wire en;
reg [7:0]key_num;

always@(posedge clk)begin
    ps2_clk_sync <= {ps2_clk_sync[1:0],ps2_clk};
end

wire sampling = ps2_clk_sync[2] & ~ps2_clk_sync[1];

always@(posedge clk)begin
    if(rst_n==0)begin//reset
        count <= 0;
        w_ptr <=0;
        r_ptr <=0;
        overflow <= 0;
        ready <=0;
        cnt <= 'd0;
    end
    else begin
        if(ready)begin//ready to output next data
            if((nextdata_n == 1'b0))//read next data
            begin
                cnt <= (cnt >= 5000000 ? 32'b0 : cnt + 1);
                if(cnt==5000000)begin
                    r_ptr <= r_ptr+3'b1;
                    if(w_ptr==(r_ptr+1'b1))//empty
                        ready <= 1'b0;
                end
            end
        end
        if(sampling)begin
            if(count==4'd10)begin
                if((buffer[0]==0) && //start bit
                   (ps2_data)     && //stop bit
                   (^buffer[9:1]))begin //odd parity
                   $display("receive %x",buffer[8:1]);
                   fifo[w_ptr] <= buffer[8:1];//kdb scan mode
                   w_ptr<= w_ptr+3'b1;
                   ready<=1;
                   overflow<= overflow | (r_ptr == (w_ptr+3'b1));
               end
               count<=0;//for next
           end else begin
               buffer[count]<=ps2_data;//store ps2_data
               count<=count+3'b1;
           end

       end
   end
   end
//   assign data=en ? fifo[r_ptr] : 'd0;//always set output data
reg [7:0]data_r;
always@(posedge clk)
    if(!rst_n)
        data_r<=0;
    else if(en)
        data_r<=fifo[r_ptr];

assign data=data_r;

   assign en = (cnt==0)&&ready&&(nextdata_n==0);

   bcd7seg u0_bcd7seg(
       .b({!ready,data[3:0]}),
       .h(seg0)
   );

   bcd7seg u1_bcd7seg(
       .b({!ready,data[7:4]}),
       .h(seg1)
   );


   rom asic_o(
//       .clk(clk),
//       .en(en),
       .addr(data),
       .dout(asic)
   );


   bcd7seg u2_bcd7seg(
       .b({!ready,asic[3:0]}),
       .h(seg2)
   );

    bcd7seg u3_bcd7seg(
        .b({!ready,asic[7:4]}),
        .h(seg3)
    );

    always@(posedge clk)
        if(!rst_n)
            key_num<=0;
        else if(en&&data==8'hf0)
            key_num<=key_num+1;

    bcd7seg u4_bcd7seg(
        .b({1'b0,key_num[3:0]}),
        .h(seg4)
    );

    bcd7seg u5_bcd7seg(
        .b({1'b0,key_num[7:4]}),
        .h(seg5)
    );
    
endmodule
