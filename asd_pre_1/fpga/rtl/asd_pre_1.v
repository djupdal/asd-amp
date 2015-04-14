// ASD PRE 1
// Toplevel

module asd_pre_1 
  (
   // Oscillators
   input wire MCU_OSC,
   input wire AUDIO_OSC,

   // MCU SPI
   output reg INT,
   input wire SCLK,
   input wire nSS,
   input wire MOSI,
   output reg MISO,

   // SPDIF routing
   input wire SPDIF_COAX,
   input wire SPDIF_TOSLINK,
   output reg SPDIF,

   // SPDIF input
   input wire EMPH,
   input wire ERROR,
   input wire SCKI1,
   input wire LRCKI1,
   input wire BCKI1,
   input wire DIN1,

   // ADC input
   input wire nOVFL,
   output reg SCKO2,
   input wire LRCKI2,
   input wire BCKI2,
   input wire DIN2,

   // DAC output
   output reg SCKO, // sys clock
   output reg LRCKO, // left/right clock
   output reg BCKO, // bit clock 
   output reg DOUT   // data out
   );

`define SOURCE_COAX 0
`define SOURCE_TOSLINK 1
`define SOURCE_ADC 2

`define SPI_CMD_MAGIC 1
`define SPI_CMD_SOURCE 2

`define ST_SPI_CMD 0
`define ST_SPI_SOURCE 1
`define ST_SPI_MAGIC 2

`define MAGIC 8'had

   reg [7:0]   spi_in;
   reg [7:0]   spi_out;
   reg [7:0]   spi_cnt;

   reg         spi_word_ready;
   reg [1:0]   source = `SOURCE_COAX;

   reg         spi_state;
   reg         next_spi_state;
   
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
         end
      end
   end

   // combinatorial
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
              `SPI_CMD_MAGIC:
                next_spi_state <= `ST_SPI_MAGIC;
            endcase
        `ST_SPI_MAGIC:
          if(spi_word_ready)
            next_spi_state <= `ST_SPI_CMD;
        `ST_SPI_SOURCE:
          if(spi_word_ready)
            next_spi_state <= `ST_SPI_CMD;
        default:
          next_spi_state <= `ST_SPI_CMD;
      endcase

      // source mux
      case(source)
        `SOURCE_COAX: begin
           SPDIF <= SPDIF_COAX;
           
           SCKO <= SCKI1;
           LRCKO <= LRCKI1;
           BCKO <= BCKI1;
           DOUT <= DIN1;
        end

        `SOURCE_TOSLINK: begin
           SPDIF <= SPDIF_TOSLINK;
           
           SCKO <= SCKI1;
           LRCKO <= LRCKI1;
           BCKO <= BCKI1;
           DOUT <= DIN1;
        end

        `SOURCE_ADC: begin
           SPDIF <= SPDIF_COAX; // dont care
           
           SCKO <= AUDIO_OSC;
           LRCKO <= LRCKI2;
           BCKO <= BCKI2;
           DOUT <= DIN2;
        end

        default: begin
           SPDIF <= SPDIF_COAX;
           
           SCKO <= SCKI1;
           LRCKO <= LRCKI1;
           BCKO <= BCKI1;
           DOUT <= DIN1;
        end
      endcase
   end

endmodule
