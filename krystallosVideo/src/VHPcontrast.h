#ifndef _CONTRAST
#define _CONTRAST

#include "ofMain.h"

//--------------------------------------------------------
class VHPcontrast {

	public:
    
        //constructor
        VHPcontrast();
    
		void				setup();
		
        ofTexture           texture[256];                           // data saved as texture
    
	private:
	
        int					calculaExponencial (int p, float e, float f);
	
		float				conversionValues	[256];              // conversion values for constants input values
        float				lowConversionValues [256];              // conversion values for constants input values
	
};

#endif