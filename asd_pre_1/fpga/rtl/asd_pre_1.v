// ASD PRE 1

`timescale 1ns/1ps

`define SCK_FREQ 256
`define BCK_FREQ 64
`define DATA_WIDTH 24

module asd_pre_1 
  (
   // Oscillators
   input wire  MCU_OSC,
   input wire  AUDIO_OSC,

   // MCU SPI
   output wire INT,
   input wire  SCLK,
   input wire  nSS,
   input wire  MOSI,
   output wire MISO,

   // SPDIF routing
   input wire  SPDIF_COAX,
   input wire  SPDIF_TOSLINK,
   output wire SPDIF,

   // SPDIF input
   input wire  EMPH,
   input wire  ERROR,
   input wire  SCKI1,
   input wire  LRCKI1,
   input wire  BCKI1,
   input wire  DIN1,

   // ADC input
   input wire  nOVFL,
   output wire SCKO2,
   input wire  LRCKI2,
   input wire  BCKI2,
   input wire  DIN2,

   // DAC output
   output wire SCKO, // sys clock
   output wire LRCKO, // left/right clock
   output wire BCKO, // bit clock 
   output wire DOUT   // data out
   );

   wire                    sck;
   wire                    bck;
   wire                    lrck_in;
   wire                    lrck_out;
   wire                    data_in;
   wire                    data_out;

   wire                    sync1;
   wire                    sync2;
   wire [`DATA_WIDTH-1:0]  left1;
   wire [`DATA_WIDTH-1:0]  left2;
   wire [`DATA_WIDTH-1:0]  right1;
   wire [`DATA_WIDTH-1:0]  right2;

   wire [1:0]              gain;

   // spi controller
   spi u_spi
     (
      .MCU_OSC(MCU_OSC),
      .AUDIO_OSC(AUDIO_OSC),

      .INT(INT),
      .SCLK(SCLK),
      .nSS(nSS),
      .MOSI(MOSI),
      .MISO(MISO),

      .SPDIF_COAX(SPDIF_COAX),
      .SPDIF_TOSLINK(SPDIF_TOSLINK),
      .SPDIF(SPDIF),

      .EMPH(EMPH),
      .ERROR(ERROR),
      .SCKI1(SCKI1),
      .LRCKI1(LRCKI1),
      .BCKI1(BCKI1),
      .DIN1(DIN1),

      .nOVFL(nOVFL),
      .SCKO2(SCKO2),
      .LRCKI2(LRCKI2),
      .BCKI2(BCKI2),
      .DIN2(DIN2),

      .sck(sck),
      .lrck(lrck_in),
      .bck(bck),
      .data(data_in),
      .gain(gain)
      );

   // audio signal path

   ser_to_par u_stp
     (
      .lrck (lrck_in),
      .bck (bck),
      .din (data_in),

      .sync (sync1),
      .left (left1),
      .right (right1)
      );
   
   gain u_gain
     (
      .bck (bck),

      .gain (gain),

      .sync_in (sync1),
      .left_in (left1),
      .right_in (right1),

      .sync_out (sync2),
      .left_out (left2),
      .right_out (right2)
      );

   par_to_ser u_pts
     (
      .bck (bck),

      .sync (sync2),
      .left (left2),
      .right (right2),

      .lrck (lrck_out),
      .dout (data_out)
      );

   assign SCKO = sck;
   assign LRCKO = lrck_out;
   assign BCKO = bck;
   assign DOUT = data_out;

endmodule

module spi
  (
   // Oscillators
   input wire       MCU_OSC,
   input wire       AUDIO_OSC,

   // MCU SPI
   output reg       INT,
   input wire       SCLK,
   input wire       nSS,
   input wire       MOSI,
   output reg       MISO,

   // SPDIF routing
   input wire       SPDIF_COAX,
   input wire       SPDIF_TOSLINK,
   output reg       SPDIF,

   // SPDIF input
   input wire       EMPH,
   input wire       ERROR,
   input wire       SCKI1,
   input wire       LRCKI1,
   input wire       BCKI1,
   input wire       DIN1,

   // ADC input
   input wire       nOVFL,
   output reg       SCKO2,
   input wire       LRCKI2,
   input wire       BCKI2,
   input wire       DIN2,

   // outputs
   output reg       sck,
   output reg       lrck,
   output wire      bck,
   output reg       data,
   output reg [1:0] gain = 0
   );

`define SOURCE_ADC 0
`define SOURCE_TOSLINK 1
`define SOURCE_COAX 2

`define SPI_CMD_MAGIC 1
`define SPI_CMD_SOURCE 2
`define SPI_CMD_GAIN 3

`define ST_SPI_CMD 0
`define ST_SPI_MAGIC 1
`define ST_SPI_SOURCE 2
`define ST_SPI_GAIN 3

`define MAGIC 8'had

   reg                     clock_source;

   reg [7:0]               spi_in;
   reg [7:0]               spi_out;
   reg [7:0]               spi_cnt;

   reg                     spi_word_ready;
   reg [1:0]               source = `SOURCE_COAX;

   reg [1:0]               spi_state;
   reg [1:0]               next_spi_state;
   
   // change SPI output on falling edge
   always @(negedge SCLK or posedge nSS) begin
      if(nSS) begin
         MISO <= 0;
      end else begin
         MISO <= spi_out[7];
      end
   end

   // clock SPI inputs on rising edge
   always @(posedge SCLK or posedge nSS) begin
      if(nSS) begin
         spi_state <= `ST_SPI_CMD;

         spi_in <= 0;
         spi_cnt <= 1;
         spi_out <= 0;
         spi_word_ready <= 0;
      end else begin
         spi_state <= next_spi_state;

         spi_in <= {spi_in[6:0],MOSI};
         spi_out <= {spi_out[6:0],1'b0};
         spi_cnt <= {spi_cnt[6:0],spi_cnt[7]};
         spi_word_ready <= spi_cnt[7];

         if(spi_cnt[7]) begin
            if({spi_in[6:0],MOSI} == `SPI_CMD_MAGIC) 
              spi_out <= `MAGIC;
            if(spi_state == `ST_SPI_SOURCE)
              source <= {spi_in[6:0],MOSI};
            if(spi_state == `ST_SPI_GAIN)
              gain <= {spi_in[0],MOSI};
         end
      end
   end

   // SPI handling
   always @* begin
      next_spi_state <= spi_state;

      INT <= 0;
      SCKO2 <= AUDIO_OSC;

      case(spi_state)
        `ST_SPI_CMD:
          if(spi_word_ready)
            case(spi_in)
              `SPI_CMD_SOURCE:
                next_spi_state <= `ST_SPI_SOURCE;
              `SPI_CMD_GAIN:
                next_spi_state <= `ST_SPI_GAIN;
              `SPI_CMD_MAGIC:
                next_spi_state <= `ST_SPI_MAGIC;
            endcase
        `ST_SPI_MAGIC:
          if(spi_word_ready)
            next_spi_state <= `ST_SPI_CMD;
        `ST_SPI_SOURCE:
          if(spi_word_ready)
            next_spi_state <= `ST_SPI_CMD;
        `ST_SPI_GAIN:
          if(spi_word_ready)
            next_spi_state <= `ST_SPI_CMD;
        default:
          next_spi_state <= `ST_SPI_CMD;
      endcase

      // source mux
      case(source)
        `SOURCE_ADC: begin
           SPDIF <= SPDIF_COAX; // dont care
           
           clock_source <= 1;
           
           sck <= AUDIO_OSC;
           lrck <= LRCKI2;
           data <= DIN2;
        end

        `SOURCE_TOSLINK: begin
           SPDIF <= SPDIF_TOSLINK;
           
           clock_source <= 0;

           sck <= SCKI1;
           lrck <= LRCKI1;
           data <= DIN1;
        end

        `SOURCE_COAX: begin
           SPDIF <= SPDIF_COAX;
           
           clock_source <= 0;

           sck <= SCKI1;
           lrck <= LRCKI1;
           data <= DIN1;
        end

        default: begin
           SPDIF <= SPDIF_COAX;
           
           clock_source <= 0;

           sck <= SCKI1;
           lrck <= LRCKI1;
           data <= DIN1;
        end
      endcase
   end

   // clock management
   BUFGMUX bck_bufg 
     (
      .I0(BCKI1),
      .I1(BCKI2),
      .S(clock_source),
      .O(bck)
      );

   BUFG mcu_bufg 
     (
      .I(MCU_OSC),
      .O(mcu_clk)
      );

endmodule

module ser_to_par
  (
   input wire                   lrck,
   input wire                   bck,
   input wire                   din,

   output reg                   sync,
   output reg [`DATA_WIDTH-1:0] left,
   output reg [`DATA_WIDTH-1:0] right
   );
   
   reg [31:0]                   sr_left;
   reg [31:0]                   sr_right;
   reg                          is_left;

   always @(posedge bck) begin
      
      // transfer to sample output registers
      if(lrck && !is_left) begin
         sync <= 1;
         right <= sr_right[31:32-`DATA_WIDTH];
         left <= sr_left[31:32-`DATA_WIDTH];
      end else begin
         sync <= 0;
      end

      // shift in new data
      if(lrck) begin
         // left channel
         sr_left <= {sr_left[30:0],din};
         is_left <= 1;
      end else begin
         // right channel
         sr_right <= {sr_right[30:0],din};
         is_left <= 0;
      end
   end

endmodule

module par_to_ser
  (
   input wire                   bck,

   input wire                   sync,
   input wire [`DATA_WIDTH-1:0] left,
   input wire [`DATA_WIDTH-1:0] right,

   output reg                   lrck,
   output wire                  dout
   );
   
   reg [`DATA_WIDTH-1:0]        shiftreg;
   reg [31:0]                   cnt;

   assign dout = shiftreg[`DATA_WIDTH-1];

   always @(negedge bck) begin
      shiftreg <= {shiftreg[`DATA_WIDTH-2:0],1'b0};
      cnt <= {cnt[30:0],1'b0};

      if(sync) begin
         shiftreg <= left;
         lrck <= 1;
         cnt <= 1;

      end else if(cnt[31]) begin
         shiftreg <= right;
         lrck <= 0;

      end
   end

endmodule

module gain
  (
   input wire                   bck,

   input wire [1:0]             gain, 

   input wire                   sync_in,
   input wire [`DATA_WIDTH-1:0] left_in,
   input wire [`DATA_WIDTH-1:0] right_in,

   output wire                  sync_out,
   output reg [`DATA_WIDTH-1:0] left_out,
   output reg [`DATA_WIDTH-1:0] right_out
   );
   
   assign sync_out = sync_in;

   reg                          overflow_left_or;
   reg                          overflow_left_and;
   reg                          overflow_right_or;
   reg                          overflow_right_and;
   reg [`DATA_WIDTH-1:0]        left_out_i;
   reg [`DATA_WIDTH-1:0]        right_out_i;

   always @* begin
      case(gain)
        0: begin
           left_out_i <= left_in;
           right_out_i <= right_in;

           overflow_left_or <= 0;
           overflow_left_and <= 1;
           overflow_right_or <= 0;
           overflow_right_and <= 1;
        end
        1: begin
           left_out_i <= {left_in[`DATA_WIDTH-2:0],1'b0};
           right_out_i <= {right_in[`DATA_WIDTH-2:0],1'b0};

           overflow_left_or <= |left_in[`DATA_WIDTH-1:`DATA_WIDTH-2];
           overflow_left_and <= &left_in[`DATA_WIDTH-1:`DATA_WIDTH-2];
           overflow_right_or <= |right_in[`DATA_WIDTH-1:`DATA_WIDTH-2];
           overflow_right_and <= &right_in[`DATA_WIDTH-1:`DATA_WIDTH-2];
        end
        2: begin
           left_out_i <= {left_in[`DATA_WIDTH-3:0],2'b0};
           right_out_i <= {right_in[`DATA_WIDTH-3:0],2'b0};

           overflow_left_or <= |left_in[`DATA_WIDTH-1:`DATA_WIDTH-3];
           overflow_left_and <= &left_in[`DATA_WIDTH-1:`DATA_WIDTH-3];
           overflow_right_or <= |right_in[`DATA_WIDTH-1:`DATA_WIDTH-3];
           overflow_right_and <= &right_in[`DATA_WIDTH-1:`DATA_WIDTH-3];
        end
        3: begin
           left_out_i <= {left_in[`DATA_WIDTH-4:0],3'b0};
           right_out_i <= {right_in[`DATA_WIDTH-4:0],3'b0};

           overflow_left_or <= |left_in[`DATA_WIDTH-1:`DATA_WIDTH-4];
           overflow_left_and <= &left_in[`DATA_WIDTH-1:`DATA_WIDTH-4];
           overflow_right_or <= |right_in[`DATA_WIDTH-1:`DATA_WIDTH-4];
           overflow_right_and <= &right_in[`DATA_WIDTH-1:`DATA_WIDTH-4];
        end
        default: begin
           left_out_i <= left_in;
           right_out_i <= right_in;

           overflow_left_or <= 0;
           overflow_left_and <= 1;
           overflow_right_or <= 0;
           overflow_right_and <= 1;
        end
      endcase
   end

   // clip signal if overflow
   always @* begin
      // left channel
      if(left_in[`DATA_WIDTH-1] && !overflow_left_and) begin
         // negative overflow
         left_out[`DATA_WIDTH-1] <= 1;
         left_out[`DATA_WIDTH-2:0] <= 0;
      end else if(!left_in[`DATA_WIDTH-1] && overflow_left_or) begin
         // positive overflow
         left_out[`DATA_WIDTH-1] <= 0;
         left_out[`DATA_WIDTH-2:0] <= ~0;
      end else begin
         left_out <= left_out_i;
      end

      // right channel
      if(right_in[`DATA_WIDTH-1] && !overflow_right_and) begin
         // negative overflow
         right_out[`DATA_WIDTH-1] <= 1;
         right_out[`DATA_WIDTH-2:0] <= 0;
      end else if(!right_in[`DATA_WIDTH-1] && overflow_right_or) begin
         // positive overflow
         right_out[`DATA_WIDTH-1] <= 0;
         right_out[`DATA_WIDTH-2:0] <= ~0;
      end else begin
         right_out <= right_out_i;
      end
   end

endmodule
