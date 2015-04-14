module asd_pre_1_tb;
   
   // Oscillators
   reg  MCU_OSC;
   reg  AUDIO_OSC;

   // MCU SPI
   wire INT;
   reg  SCLK;
   reg  nSS;
   reg  MOSI;
   wire  MISO;

   // SPDIF routing
   reg  SPDIF_COAX;
   reg  SPDIF_TOSLINK;
   wire  SPDIF;

   // SPDIF input
   reg  EMPH;
   reg  ERROR;
   reg  SCKI1;
   reg  LRCKI1;
   reg  BCKI1;
   reg  DIN1;

   // ADC input
   reg  nOVFL;
   wire SCKO2;
   reg  LRCKI2;
   reg  BCKI2;
   reg  DIN2;

   // DAC output
   wire  SCKO; // sys clock
   wire  LRCKO; // left/right clock
   wire  BCKO; // bit clock 
   wire  DOUT; // data out

   /////////////////////////////////////////////////////////////////////////////

   asd_pre_1 dut
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
      .SCKO(SCKO),
      .LRCKO(LRCKO),
      .BCKO(BCKO),
      .DOUT(DOUT)
      );

   /////////////////////////////////////////////////////////////////////////////

   initial
     begin
        // Oscillators
        MCU_OSC <= 0;
        AUDIO_OSC <= 0;
        
        // MCU SPI
        SCLK <= 0;
        nSS <= 1;
        MOSI <= 0;
        
        // SPDIF routing
        SPDIF_COAX <= 0;
        SPDIF_TOSLINK <= 0;
        
        // SPDIF input
        EMPH <= 0;
        ERROR <= 0;
        SCKI1 <= 0;
        LRCKI1 <= 0;
        BCKI1 <= 0;
        DIN1 <= 0;
        
        // ADC input
        nOVFL <= 0;
        LRCKI2 <= 0;
        BCKI2 <= 0;
        DIN2 <= 0;
        
        #1000 nSS <= 0;
        #1000 MOSI <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;
        MOSI <= 1;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;
        MOSI <= 0;

        //

        #5000 MOSI <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;
        MOSI <= 1;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;
        MOSI <= 0;

        #1000 SCLK <= 1;
        #1000 SCLK <= 0;

        #1000 nSS <= 1;

        //

     end

endmodule
