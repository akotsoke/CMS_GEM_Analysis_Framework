//
//  parameterLoaderAmoreSRS.cpp
//  
//
//  Created by Brian L Dorney on 26/01/16.
//
//

#include "DetectorMPGD.h"
#include "ParameterLoaderAmoreSRS.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

using QualityControl::Timing::getString;
using QualityControl::Timing::printStreamStatus;
using QualityControl::Timing::stofSafe;
using QualityControl::Timing::stoiSafe;

using namespace QualityControl::Uniformity;

//Default Constructor
ParameterLoaderAmoreSRS::ParameterLoaderAmoreSRS(){
    m_bVerboseMode_IO = false;
} //End default constructor

//Load the amore mapping file and extract the detector parameters
void ParameterLoaderAmoreSRS::loadAmoreMapping(string & strInputMappingFileName){
    //Variable Declaration
    string strLine; //Input Line from File
    
    vector<string> vec_strParam; //vector of parameters found from strLine
    
    //Open the Mapping File
    //------------------------------------------------------
    /*if (m_bVerboseMode_IO) { //Case: User Requested Verbose Error Messages - I/O
        cout<< "treeProducerTDC::readRuns(): trying to open and read: " << strInputMappingFileName << endl;
    } //End Case: User Requested Verbose Error Messages - I/O
    
    ifstream file_AmoreMapping( strInputMappingFileName.c_str() );
    
    //Check to See if Mapping File Opened Successfully
    //------------------------------------------------------
    if (!file_AmoreMapping.is_open() && m_bVerboseMode_IO) {
        perror( ("treeProducerTDC::readRuns(): error while opening file: " + strInputMappingFileName).c_str() );
        printStreamStatus(file_AmoreMapping);
    }*/
    
    //ifstream file_AmoreMapping = getFileStream(strInputMappingFileName, m_bVerboseMode_IO);
    ifstream file_AmoreMapping;
    setFileStream(strInputMappingFileName, file_AmoreMapping, m_bVerboseMode_IO);    

    //Loop Over data Input File
    //------------------------------------------------------
    //Read the file via std::getline().  Obey good coding practice rules:
    //  -first the I/O operation, then error check, then data processing
    //  -failbit and badbit prevent data processing, eofbit does not
    //See: http://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/
    while ( getline( file_AmoreMapping, strLine ) ){ //Loop Over input mapping file
        //Does the user want to comment out this run?
        if ( strLine.compare(0,1,"#") == 0 ) continue;
        
        //transform to all uppercase (should be already but...)
        std::transform(strLine.begin(),strLine.end(),strLine.begin(),toupper);
        
        if ( strLine.find("DET") != string::npos ) { //Case: Detector Mapping
            //Parse the input line
            vec_strParam = Timing::getCharSeparatedList(strLine,',');
            
            //Debugging
            //cout<<"Lines from " << strInputMappingFileName << endl;
            /*for (int i=0; i < vec_strParam.size(); ++i) {
                cout<<i<<"\t"<<vec_strParam[i]<<endl;
            }*/
            
            //expecting a tab+comma separated list ordered as:
            //#Header   ReadoutType  DetType    DetName    Sector     SectPos   SectSize   nbConnect  orient
            //(getCharSeparatedList should strip the tab's out)
            if (vec_strParam.size() == 9) { //Case: Correct Number of Parameters
                int iEta = stoiSafe( getStringOnlyNumeric(vec_strParam[4]) );
                
                det_GE11.setEtaSector(iEta, stofSafe( vec_strParam[5]), stofSafe( vec_strParam[6] ), stoiSafe( vec_strParam[7] ) );
            } //End Case: Correct Number of Parameters
            else{ //Case: Incorrect Number of Parameters
                printClassMethodMsg("ParameterLoaderAmoreSRS","loadAmoreMapping",("Error! - Found only " + getString(vec_strParam.size() ) + " Parameters (Expected 9) for line:").c_str() );
                printClassMethodMsg("ParameterLoaderAmoreSRS","loadAmoreMapping",("\t" + strLine).c_str() );
                printClassMethodMsg("ParameterLoaderAmoreSRS","loadAmoreMapping",("\tPlease Cross-Check Amore SRS Mapping File: " + strInputMappingFileName ).c_str() );
                
                continue;
            } //End Case: Incorrect Number of Parameters
        } //End Case: Detector Mapping
        else if ( strLine.find("APV") != string::npos ) { //Case: APV Mapping
            //For now this information is not used, exit
            break;
        } //End Case: APV Mapping
    } //End Loop Over input mapping file
    
    //Check to see if we had problems while reading the file
    if (file_AmoreMapping.bad() && m_bVerboseMode_IO) {
        perror( ("Uniformity::ParameterLoaderAmoreSRS::loadAmoreMapping(): error while reading file: " + strInputMappingFileName).c_str() );
        printStreamStatus(file_AmoreMapping);
    }
    
    //Close the file
    file_AmoreMapping.close();
    
    return;
} //End ParameterLoaderAmoreSRS::loadAmoreMapping()
