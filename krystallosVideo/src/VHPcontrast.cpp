#include "VHPcontrast.h"

//constructor

//----------------------------------------------------------------
VHPcontrast::VHPcontrast() {
}

// methods

//--------------------------------------------------------------
void VHPcontrast::setup(){
	
	// conversion float values for input 0-255 values to use in functiona as constants
	for (int i=0; i<32; i++) {
		conversionValues[i] = 0.01 + 0.01*i;
	}
	for (int i=32; i<64; i++) {
		conversionValues[i] = conversionValues[31] + 0.02*(i-31);
	}
	for (int i=64; i<96; i++) {
		conversionValues[i] = conversionValues[63] + 0.04*(i-63);
	}
	for (int i=96; i<128; i++) {
		conversionValues[i] = conversionValues[95] + 0.08*(i-95);
	}
	for (int i=128; i<160; i++) {
		conversionValues[i] = conversionValues[127] + 0.16*(i-127);
	}
	for (int i=160; i<192; i++) {
		conversionValues[i] = conversionValues[159] + 0.32*(i-159);
	}
	for (int i=192; i<224; i++) {
		conversionValues[i] = conversionValues[191] + 0.64*(i-191);
	}
	for (int i=224; i<256; i++) {
		conversionValues[i] = conversionValues[223] + 1.28*(i-223);
	}
	// conversion float values for input 0-255 values to use in functiona as constants
	for (int i=0; i<64; i++) {
		lowConversionValues[i] = 0.01 + 0.01*i;
	}
	for (int i=64; i<128; i++) {
		lowConversionValues[i] = lowConversionValues[63] + 0.02*(i-63);
	}
	for (int i=128; i<192; i++) {
		lowConversionValues[i] = lowConversionValues[127] + 0.04*(i-127);
	}
	for (int i=192; i<256; i++) {
		lowConversionValues[i] = lowConversionValues[191] + 0.08*(i-191);
	}
	
	// buffered calculations in a matrix for contrasteLineal, contrasteCoseno, contrasteExponencial
    unsigned char * val;
	for (int e=0; e<256; e++) {
        texture[e].allocate(256,256, GL_RGB);
        val = new unsigned char[256*256*3];
        for (int f=0; f<256; f++) {
            for (int i=0; i<256; i++) {
                int pos = f*256*3 + i*3;
                val[pos] = calculaExponencial(i, conversionValues[e], conversionValues[f]*10.0);
                val[pos+1] = calculaExponencial(i, conversionValues[e], conversionValues[f]*10.0);
                val[pos+2] = calculaExponencial(i, conversionValues[e], conversionValues[f]*10.0);
                //cout << calculaExponencial(i, conversionValues[e], conversionValues[f]*10.0) << endl;
            }
        }
        texture[e].loadData(val, 256,256, GL_RGB);
    }
}


//--------------------------------------------------------------
// C(x) = 255 / ((((255-x)*f)^e)+1)
//--------------------------------------------------------------
int VHPcontrast::calculaExponencial(int p, float e, float f){
	float min_rgb_value = 255/(pow(255/f,e)+1);
	float rgb_value = 255/(pow((255-p)/f,e)+1);
	rgb_value = 255 * (rgb_value - min_rgb_value)/(255 - min_rgb_value);
	int resultado = rgb_value;
	return resultado;
}
