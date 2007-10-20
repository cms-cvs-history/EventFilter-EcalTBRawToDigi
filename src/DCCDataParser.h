/*----------------------------------------------------------*/
/* DCC DATA PARSER                                          */
/*                                                          */
/* Author : N.Almeida (LIP)         Date   : 30/05/2004     */
/*----------------------------------------------------------*/

#ifndef DCCTBDATAPARSER_HH
#define DCCTBDATAPARSER_HH

#include <fstream>                   //STL
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <stdio.h>                     //C

#include "ECALParserException.h"      //DATA DECODER
#include "DCCEventBlock.h"
#include "DCCDataMapper.h"


class DCCTBDataMapper;
class DCCTBEventBlock;


class DCCTBDataParser{

public : 
  
  /**
     Class constructor: takes a vector of 10 parameters and flags for parseInternalData and debug
     Parameters are: 
     0 - crystal samples (default is 10)
     1 - number of trigger time samples (default is 1)
     2 - number of TT (default is 68)
     3 - number of SR Flags (default is 68)
     4 - DCC id
     5 - SR id
     [6-9] - TCC[6-9] id
  */
  DCCTBDataParser( std::vector<ulong> parserParameters , bool parseInternalData = true, bool debug = true);
  
  /**
    Parse data from file 
  */
  void parseFile( std::string fileName, bool singleEvent = false);
	
  /**
     Parse data from a buffer
  */
  void parseBuffer( ulong * buffer, ulong bufferSize, bool singleEvent = false);

  /**
     Get method for DCCTBDataMapper
  */
  DCCTBDataMapper *mapper();
		
  /**
     Check if EVENT LENGTH is coeherent and if BOE/EOE are correctly written
     returns 3 bits code with the error found + event length
  */
  std::pair<ulong,ulong> checkEventLength(ulong * pointerToEvent, ulong bytesToEnd, bool singleEvent = false);
  
  /**
     Get methods for parser parameters;
  */
  std::vector<ulong> parserParameters(); 
  ulong numbXtalSamples();
  ulong numbTriggerSamples();
  ulong numbTTs();
  ulong numbSRF();
  ulong dccId();
  ulong srpId();
  ulong tcc1Id();
  ulong tcc2Id();
  ulong tcc3Id();
  ulong tcc4Id();
  

  /**
     Set method for parser parameters
  */
  void  setParameters( std::vector<ulong> newParameters );


  /**
     Get methods for block sizes
  */
  ulong srpBlockSize();
  ulong tccBlockSize();

  /**
     Get methods for debug flag
  */
  bool  debug();

  /**
     Get method for DCCEventBlocks vector
   */
  std::vector<DCCTBEventBlock *> & dccEvents();

  /**
     Get method for error counters map
  */
  std::map<std::string,ulong> & errorCounters();

  /**
   * Get method for events
   */
  std::vector< std::pair< ulong, std::pair<ulong *, ulong> > > events();


  /**
     Reset Error Counters
  */
  void resetErrorCounters();
		

  /**
     Methods to get data strings formatted as decimal/hexadecimal, indexes and indexed data
  */
  std::string getDecString(ulong data);		
  std::string getHexString(ulong data);
  std::string index(ulong position);
  std::string getIndexedData( ulong indexed, ulong * pointer);

  /**
   * Retrieves a pointer to the data buffer
   */
  ulong *getBuffer() { return buffer_;}
  
  /**
     Class destructor
  */
  ~DCCTBDataParser();

  enum DCCDataParserGlobalFields{
    EMPTYEVENTSIZE = 32                   //bytes
  };
 
protected :
  void computeBlockSizes();

  ulong *buffer_;                //data buffer
  ulong bufferSize_;             //buffer size

  ulong srpBlockSize_;           //SR block size
  ulong tccBlockSize_;           //TCC block size

  ulong processedEvent_;
  std::string eventErrors_;
  DCCTBDataMapper *mapper_;
  
  std::vector<DCCTBEventBlock *> dccEvents_;
  
  // std::pair< errorMask, std::pair< pointer to event, event size (number of DW)> >
  std::vector< std::pair< ulong, std::pair<ulong *, ulong> > > events_;
  
  bool parseInternalData_;          //parse internal data flag
  bool debug_;                      //debug flag
  std::map<std::string,ulong> errors_;        //errors map
  std::vector<ulong> parameters;         //parameters vector

  enum DCCTBDataParserFields{
    EVENTLENGTHMASK = 0xFFFFFF,
    
    BOEBEGIN = 28,                  //begin of event (on 32 bit string starts at bit 28)
    BOEMASK = 0xF,                  //mask is 4 bits (F)
    BOE =0x5,                       //B'0101'

    EOEBEGIN = 28,                  //end of event
    EOEMASK = 0xF,                  //4 bits
    EOE =0xA                        //B'1010'
  };
		
};

inline DCCTBDataMapper *DCCTBDataParser::mapper() { return mapper_;}

inline std::vector<ulong> DCCTBDataParser::parserParameters() { return parameters; }
inline ulong DCCTBDataParser::numbXtalSamples()     { return parameters[0]; }
inline ulong DCCTBDataParser::numbTriggerSamples()  { return parameters[1]; }
inline ulong DCCTBDataParser::numbTTs()             { return parameters[2]; }
inline ulong DCCTBDataParser::numbSRF()             { return parameters[3]; }
inline ulong DCCTBDataParser::dccId()               { return parameters[4]; }
inline ulong DCCTBDataParser::srpId()               { return parameters[5]; }
inline ulong DCCTBDataParser::tcc1Id()              { return parameters[6]; } 
inline ulong DCCTBDataParser::tcc2Id()              { return parameters[7]; } 
inline ulong DCCTBDataParser::tcc3Id()              { return parameters[8]; } 
inline ulong DCCTBDataParser::tcc4Id()              { return parameters[9]; }

inline void  DCCTBDataParser::setParameters( std::vector<ulong> newParameters ){ parameters = newParameters; computeBlockSizes();}

inline ulong DCCTBDataParser::srpBlockSize()        { return srpBlockSize_; } 
inline ulong DCCTBDataParser::tccBlockSize()        { return tccBlockSize_; } 

inline bool DCCTBDataParser::debug()                          { return debug_;     }
inline std::vector<DCCTBEventBlock *> &DCCTBDataParser::dccEvents()  { return dccEvents_;    }
inline std::map<std::string,ulong> &DCCTBDataParser::errorCounters()    { return errors_;       }
inline std::vector< std::pair< ulong, std::pair<ulong *, ulong> > > DCCTBDataParser::events() { return events_;   }


#endif

