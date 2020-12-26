#ifdef USE_UPDATE_SERVER

// minified code based on https://github.com/Gheotic/ESP-HTML-Compressor
const PROGMEM char UpdateServerIndex[] = {0X3C,0X68,0X74,0X6D,0X6C,0X20,0X63,0X68,0X61,0X72,0X73,0X65,0X74,0X3D,0X55,0X54,0X46,0X2D,0X38,0X3E,0XA,0X3C,0X73,0X74,0X79,0X6C,0X65,0X3E,0X62,0X6F,0X64,0X79,0X7B,0X62,0X61,0X63,0X6B,0X67,0X72,0X6F,0X75,0X6E,0X64,0X2D,0X63,0X6F,0X6C,0X6F,0X72,0X3A,0X23,0X64,0X64,0X64,0X7D,0X3C,0X2F,0X73,0X74,0X79,0X6C,0X65,0X3E,0XA,0X3C,0X62,0X6F,0X64,0X79,0X3E,0XA,0X3C,0X68,0X31,0X3E,0X44,0X53,0X4D,0X52,0X2D,0X6C,0X6F,0X67,0X67,0X65,0X72,0X20,0X46,0X6C,0X61,0X73,0X68,0X20,0X75,0X74,0X69,0X6C,0X69,0X74,0X79,0X3C,0X2F,0X68,0X31,0X3E,0XA,0X3C,0X66,0X6F,0X72,0X6D,0X20,0X6D,0X65,0X74,0X68,0X6F,0X64,0X3D,0X50,0X4F,0X53,0X54,0X20,0X61,0X63,0X74,0X69,0X6F,0X6E,0X3D,0X22,0X3F,0X63,0X6D,0X64,0X3D,0X30,0X22,0X20,0X65,0X6E,0X63,0X74,0X79,0X70,0X65,0X3D,0X6D,0X75,0X6C,0X74,0X69,0X70,0X61,0X72,0X74,0X2F,0X66,0X6F,0X72,0X6D,0X2D,0X64,0X61,0X74,0X61,0X3E,0XA,0X53,0X65,0X6C,0X65,0X63,0X74,0X65,0X65,0X72,0X20,0X65,0X65,0X6E,0X20,0X22,0X3C,0X62,0X3E,0X2E,0X69,0X6E,0X6F,0X2E,0X62,0X69,0X6E,0X3C,0X2F,0X62,0X3E,0X22,0X20,0X62,0X65,0X73,0X74,0X61,0X6E,0X64,0X3C,0X62,0X72,0X3E,0XA,0X3C,0X69,0X6E,0X70,0X75,0X74,0X20,0X74,0X79,0X70,0X65,0X3D,0X66,0X69,0X6C,0X65,0X20,0X61,0X63,0X63,0X65,0X70,0X74,0X3D,0X69,0X6E,0X6F,0X2E,0X62,0X69,0X6E,0X20,0X6E,0X61,0X6D,0X65,0X3D,0X66,0X69,0X72,0X6D,0X77,0X61,0X72,0X65,0X3E,0XA,0X3C,0X69,0X6E,0X70,0X75,0X74,0X20,0X74,0X79,0X70,0X65,0X3D,0X73,0X75,0X62,0X6D,0X69,0X74,0X20,0X76,0X61,0X6C,0X75,0X65,0X3D,0X22,0X46,0X6C,0X61,0X73,0X68,0X20,0X46,0X69,0X72,0X6D,0X77,0X61,0X72,0X65,0X22,0X3E,0XA,0X3C,0X2F,0X66,0X6F,0X72,0X6D,0X3E,0XA,0X3C,0X66,0X6F,0X72,0X6D,0X20,0X6D,0X65,0X74,0X68,0X6F,0X64,0X3D,0X50,0X4F,0X53,0X54,0X20,0X61,0X63,0X74,0X69,0X6F,0X6E,0X3D,0X22,0X3F,0X63,0X6D,0X64,0X3D,0X31,0X30,0X30,0X22,0X20,0X65,0X6E,0X63,0X74,0X79,0X70,0X65,0X3D,0X6D,0X75,0X6C,0X74,0X69,0X70,0X61,0X72,0X74,0X2F,0X66,0X6F,0X72,0X6D,0X2D,0X64,0X61,0X74,0X61,0X3E,0XA,0X53,0X65,0X6C,0X65,0X63,0X74,0X65,0X65,0X72,0X20,0X65,0X65,0X6E,0X20,0X22,0X3C,0X62,0X3E,0X2E,0X73,0X70,0X69,0X66,0X66,0X73,0X2E,0X62,0X69,0X6E,0X3C,0X2F,0X62,0X3E,0X22,0X20,0X62,0X65,0X73,0X74,0X61,0X6E,0X64,0X3C,0X62,0X72,0X3E,0XA,0X3C,0X69,0X6E,0X70,0X75,0X74,0X20,0X74,0X79,0X70,0X65,0X3D,0X66,0X69,0X6C,0X65,0X20,0X61,0X63,0X63,0X65,0X70,0X74,0X3D,0X73,0X70,0X69,0X66,0X66,0X73,0X2E,0X62,0X69,0X6E,0X20,0X6E,0X61,0X6D,0X65,0X3D,0X66,0X69,0X6C,0X65,0X73,0X79,0X73,0X74,0X65,0X6D,0X3E,0XA,0X3C,0X69,0X6E,0X70,0X75,0X74,0X20,0X74,0X79,0X70,0X65,0X3D,0X73,0X75,0X62,0X6D,0X69,0X74,0X20,0X76,0X61,0X6C,0X75,0X65,0X3D,0X22,0X46,0X6C,0X61,0X73,0X68,0X20,0X53,0X70,0X69,0X66,0X66,0X73,0X22,0X3E,0XA,0X3C,0X2F,0X66,0X6F,0X72,0X6D,0X3E,0XA,0X3C,0X68,0X72,0X3E,0XA,0X3C,0X62,0X72,0X3E,0X3C,0X66,0X6F,0X6E,0X74,0X20,0X63,0X6F,0X6C,0X6F,0X72,0X3D,0X72,0X65,0X64,0X3E,0X4C,0X45,0X54,0X20,0X4F,0X50,0X21,0X21,0X21,0X3C,0X2F,0X66,0X6F,0X6E,0X74,0X3E,0XA,0X3C,0X62,0X72,0X3E,0X42,0X69,0X6A,0X20,0X68,0X65,0X74,0X20,0X66,0X6C,0X61,0X73,0X68,0X65,0X6E,0X20,0X76,0X61,0X6E,0X20,0X53,0X50,0X49,0X46,0X46,0X53,0X20,0X72,0X61,0X61,0X6B,0X74,0X20,0X75,0X20,0X64,0X65,0X20,0X52,0X49,0X4E,0X47,0X2D,0X62,0X65,0X73,0X74,0X61,0X6E,0X64,0X65,0X6E,0X20,0X6B,0X77,0X69,0X6A,0X74,0X2E,0XA,0X3C,0X62,0X72,0X3E,0X4D,0X61,0X61,0X6B,0X20,0X64,0X61,0X61,0X72,0X6F,0X6D,0X20,0X65,0X65,0X72,0X73,0X74,0X20,0X65,0X65,0X6E,0X20,0X6B,0X6F,0X70,0X69,0X65,0X20,0X76,0X61,0X6E,0X20,0X64,0X65,0X7A,0X65,0X20,0X62,0X65,0X73,0X74,0X61,0X6E,0X64,0X65,0X6E,0X20,0X28,0X76,0X69,0X61,0X20,0X62,0X65,0X73,0X74,0X61,0X6E,0X64,0X73,0X62,0X65,0X68,0X65,0X65,0X72,0X29,0X20,0X65,0X6E,0X20,0X7A,0X65,0X74,0X20,0X64,0X65,0X20,0X64,0X61,0X74,0X61,0X2D,0X62,0X65,0X73,0X74,0X61,0X6E,0X64,0X65,0X6E,0X20,0X6E,0X61,0X20,0X68,0X65,0X74,0X20,0X66,0X6C,0X61,0X73,0X68,0X65,0X6E,0X20,0X76,0X61,0X6E,0X20,0X53,0X50,0X49,0X46,0X46,0X53,0X20,0X77,0X65,0X65,0X72,0X20,0X74,0X65,0X72,0X75,0X67,0X2E,0XA,0X3C,0X68,0X72,0X3E,0XA,0X3C,0X62,0X72,0X3E,0XA,0X3C,0X62,0X72,0X3E,0X57,0X61,0X63,0X68,0X74,0X20,0X6E,0X6F,0X67,0X20,0X3C,0X73,0X70,0X61,0X6E,0X20,0X73,0X74,0X79,0X6C,0X65,0X3D,0X66,0X6F,0X6E,0X74,0X2D,0X73,0X69,0X7A,0X65,0X3A,0X31,0X2E,0X33,0X65,0X6D,0X20,0X69,0X64,0X3D,0X77,0X61,0X69,0X74,0X53,0X65,0X63,0X6F,0X6E,0X64,0X73,0X3E,0X31,0X32,0X30,0X3C,0X2F,0X73,0X70,0X61,0X6E,0X3E,0X20,0X73,0X65,0X63,0X6F,0X6E,0X64,0X65,0X6E,0X20,0X2E,0X2E,0XA,0X3C,0X62,0X72,0X3E,0X41,0X6C,0X73,0X20,0X68,0X65,0X74,0X20,0X6C,0X69,0X6A,0X6B,0X74,0X20,0X6F,0X66,0X20,0X65,0X72,0X20,0X6E,0X69,0X65,0X74,0X73,0X20,0X67,0X65,0X62,0X65,0X75,0X72,0X64,0X2C,0X20,0X77,0X61,0X63,0X68,0X74,0X20,0X64,0X61,0X6E,0X20,0X74,0X6F,0X74,0X20,0X64,0X65,0X20,0X74,0X65,0X6C,0X6C,0X65,0X72,0X20,0X6F,0X70,0X20,0X27,0X6E,0X75,0X6C,0X27,0X20,0X73,0X74,0X61,0X61,0X74,0X20,0X65,0X6E,0X20,0X6B,0X6C,0X69,0X6B,0X20,0X64,0X61,0X61,0X72,0X6E,0X61,0X20,0X3C,0X73,0X70,0X61,0X6E,0X20,0X73,0X74,0X79,0X6C,0X65,0X3D,0X66,0X6F,0X6E,0X74,0X2D,0X73,0X69,0X7A,0X65,0X3A,0X31,0X2E,0X33,0X65,0X6D,0X3E,0X3C,0X62,0X3E,0X3C,0X61,0X20,0X68,0X72,0X65,0X66,0X3D,0X2F,0X20,0X3E,0X68,0X69,0X65,0X72,0X3C,0X2F,0X61,0X3E,0X3C,0X2F,0X62,0X3E,0X3C,0X2F,0X73,0X70,0X61,0X6E,0X3E,0X21,0XA,0X3C,0X2F,0X62,0X6F,0X64,0X79,0X3E,0XA,0X3C,0X73,0X63,0X72,0X69,0X70,0X74,0X3E,0X76,0X61,0X72,0X20,0X73,0X3D,0X64,0X6F,0X63,0X75,0X6D,0X65,0X6E,0X74,0X2E,0X67,0X65,0X74,0X45,0X6C,0X65,0X6D,0X65,0X6E,0X74,0X42,0X79,0X49,0X64,0X28,0X22,0X77,0X61,0X69,0X74,0X53,0X65,0X63,0X6F,0X6E,0X64,0X73,0X22,0X29,0X2E,0X74,0X65,0X78,0X74,0X43,0X6F,0X6E,0X74,0X65,0X6E,0X74,0X2C,0X63,0X6F,0X75,0X6E,0X74,0X64,0X6F,0X77,0X6E,0X3D,0X73,0X65,0X74,0X49,0X6E,0X74,0X65,0X72,0X76,0X61,0X6C,0X28,0X66,0X75,0X6E,0X63,0X74,0X69,0X6F,0X6E,0X28,0X29,0X7B,0X73,0X2D,0X2D,0X2C,0X28,0X64,0X6F,0X63,0X75,0X6D,0X65,0X6E,0X74,0X2E,0X67,0X65,0X74,0X45,0X6C,0X65,0X6D,0X65,0X6E,0X74,0X42,0X79,0X49,0X64,0X28,0X22,0X77,0X61,0X69,0X74,0X53,0X65,0X63,0X6F,0X6E,0X64,0X73,0X22,0X29,0X2E,0X74,0X65,0X78,0X74,0X43,0X6F,0X6E,0X74,0X65,0X6E,0X74,0X3D,0X73,0X29,0X3C,0X3D,0X30,0X26,0X26,0X28,0X63,0X6C,0X65,0X61,0X72,0X49,0X6E,0X74,0X65,0X72,0X76,0X61,0X6C,0X28,0X63,0X6F,0X75,0X6E,0X74,0X64,0X6F,0X77,0X6E,0X29,0X2C,0X77,0X69,0X6E,0X64,0X6F,0X77,0X2E,0X6C,0X6F,0X63,0X61,0X74,0X69,0X6F,0X6E,0X2E,0X61,0X73,0X73,0X69,0X67,0X6E,0X28,0X22,0X2F,0X22,0X29,0X29,0X7D,0X2C,0X31,0X65,0X33,0X29,0X3C,0X2F,0X73,0X63,0X72,0X69,0X70,0X74,0X3E,0XA,0X3C,0X2F,0X68,0X74,0X6D,0X6C,0X3E};
const PROGMEM char UpdateServerSuccess[] = {0X3C,0X68,0X74,0X6D,0X6C,0X20,0X63,0X68,0X61,0X72,0X73,0X65,0X74,0X3D,0X55,0X54,0X46,0X2D,0X38,0X3E,0XA,0X3C,0X73,0X74,0X79,0X6C,0X65,0X3E,0X62,0X6F,0X64,0X79,0X7B,0X62,0X61,0X63,0X6B,0X67,0X72,0X6F,0X75,0X6E,0X64,0X2D,0X63,0X6F,0X6C,0X6F,0X72,0X3A,0X23,0X64,0X64,0X64,0X7D,0X3C,0X2F,0X73,0X74,0X79,0X6C,0X65,0X3E,0XA,0X3C,0X62,0X6F,0X64,0X79,0X3E,0XA,0X3C,0X68,0X31,0X3E,0X44,0X53,0X4D,0X52,0X2D,0X6C,0X6F,0X67,0X67,0X65,0X72,0X20,0X46,0X6C,0X61,0X73,0X68,0X20,0X75,0X74,0X69,0X6C,0X69,0X74,0X79,0X3C,0X2F,0X68,0X31,0X3E,0XA,0X3C,0X62,0X72,0X3E,0XA,0X3C,0X68,0X32,0X3E,0X55,0X70,0X64,0X61,0X74,0X65,0X20,0X73,0X75,0X63,0X63,0X65,0X73,0X73,0X66,0X75,0X6C,0X21,0X3C,0X2F,0X68,0X32,0X3E,0XA,0X3C,0X62,0X72,0X3E,0XA,0X3C,0X62,0X72,0X3E,0X57,0X61,0X69,0X74,0X20,0X66,0X6F,0X72,0X20,0X74,0X68,0X65,0X20,0X44,0X53,0X4D,0X52,0X2D,0X6C,0X6F,0X67,0X67,0X65,0X72,0X20,0X74,0X6F,0X20,0X72,0X65,0X62,0X6F,0X6F,0X74,0X20,0X61,0X6E,0X64,0X20,0X73,0X74,0X61,0X72,0X74,0X20,0X74,0X68,0X65,0X20,0X48,0X54,0X54,0X50,0X20,0X73,0X65,0X72,0X76,0X65,0X72,0XA,0X3C,0X62,0X72,0X3E,0XA,0X3C,0X62,0X72,0X3E,0XA,0X3C,0X62,0X72,0X3E,0X57,0X61,0X63,0X68,0X74,0X20,0X6E,0X6F,0X67,0X20,0X3C,0X73,0X70,0X61,0X6E,0X20,0X73,0X74,0X79,0X6C,0X65,0X3D,0X66,0X6F,0X6E,0X74,0X2D,0X73,0X69,0X7A,0X65,0X3A,0X31,0X2E,0X33,0X65,0X6D,0X20,0X69,0X64,0X3D,0X77,0X61,0X69,0X74,0X53,0X65,0X63,0X6F,0X6E,0X64,0X73,0X3E,0X36,0X30,0X3C,0X2F,0X73,0X70,0X61,0X6E,0X3E,0X20,0X73,0X65,0X63,0X6F,0X6E,0X64,0X65,0X6E,0X20,0X2E,0X2E,0XA,0X3C,0X62,0X72,0X3E,0X41,0X6C,0X73,0X20,0X68,0X65,0X74,0X20,0X6C,0X69,0X6A,0X6B,0X74,0X20,0X6F,0X66,0X20,0X65,0X72,0X20,0X6E,0X69,0X65,0X74,0X73,0X20,0X67,0X65,0X62,0X65,0X75,0X72,0X64,0X2C,0X20,0X77,0X61,0X63,0X68,0X74,0X20,0X64,0X61,0X6E,0X20,0X74,0X6F,0X74,0X20,0X64,0X65,0X20,0X74,0X65,0X6C,0X6C,0X65,0X72,0X20,0X6F,0X70,0X20,0X27,0X6E,0X75,0X6C,0X27,0X20,0X73,0X74,0X61,0X61,0X74,0X20,0X65,0X6E,0X20,0X6B,0X6C,0X69,0X6B,0X20,0X64,0X61,0X61,0X72,0X6E,0X61,0X20,0X3C,0X73,0X70,0X61,0X6E,0X20,0X73,0X74,0X79,0X6C,0X65,0X3D,0X66,0X6F,0X6E,0X74,0X2D,0X73,0X69,0X7A,0X65,0X3A,0X31,0X2E,0X33,0X65,0X6D,0X3E,0X3C,0X62,0X3E,0X3C,0X61,0X20,0X68,0X72,0X65,0X66,0X3D,0X2F,0X20,0X3E,0X68,0X69,0X65,0X72,0X3C,0X2F,0X61,0X3E,0X3C,0X2F,0X62,0X3E,0X3C,0X2F,0X73,0X70,0X61,0X6E,0X3E,0X21,0XA,0X3C,0X2F,0X62,0X6F,0X64,0X79,0X3E,0XA,0X3C,0X73,0X63,0X72,0X69,0X70,0X74,0X3E,0X76,0X61,0X72,0X20,0X73,0X3D,0X64,0X6F,0X63,0X75,0X6D,0X65,0X6E,0X74,0X2E,0X67,0X65,0X74,0X45,0X6C,0X65,0X6D,0X65,0X6E,0X74,0X42,0X79,0X49,0X64,0X28,0X22,0X77,0X61,0X69,0X74,0X53,0X65,0X63,0X6F,0X6E,0X64,0X73,0X22,0X29,0X2E,0X74,0X65,0X78,0X74,0X43,0X6F,0X6E,0X74,0X65,0X6E,0X74,0X2C,0X63,0X6F,0X75,0X6E,0X74,0X64,0X6F,0X77,0X6E,0X3D,0X73,0X65,0X74,0X49,0X6E,0X74,0X65,0X72,0X76,0X61,0X6C,0X28,0X66,0X75,0X6E,0X63,0X74,0X69,0X6F,0X6E,0X28,0X29,0X7B,0X73,0X2D,0X2D,0X2C,0X28,0X64,0X6F,0X63,0X75,0X6D,0X65,0X6E,0X74,0X2E,0X67,0X65,0X74,0X45,0X6C,0X65,0X6D,0X65,0X6E,0X74,0X42,0X79,0X49,0X64,0X28,0X22,0X77,0X61,0X69,0X74,0X53,0X65,0X63,0X6F,0X6E,0X64,0X73,0X22,0X29,0X2E,0X74,0X65,0X78,0X74,0X43,0X6F,0X6E,0X74,0X65,0X6E,0X74,0X3D,0X73,0X29,0X3C,0X3D,0X30,0X26,0X26,0X28,0X63,0X6C,0X65,0X61,0X72,0X49,0X6E,0X74,0X65,0X72,0X76,0X61,0X6C,0X28,0X63,0X6F,0X75,0X6E,0X74,0X64,0X6F,0X77,0X6E,0X29,0X2C,0X77,0X69,0X6E,0X64,0X6F,0X77,0X2E,0X6C,0X6F,0X63,0X61,0X74,0X69,0X6F,0X6E,0X2E,0X61,0X73,0X73,0X69,0X67,0X6E,0X28,0X22,0X2F,0X22,0X29,0X29,0X7D,0X2C,0X31,0X65,0X33,0X29,0X3C,0X2F,0X73,0X63,0X72,0X69,0X70,0X74,0X3E,0XA,0X3C,0X2F,0X68,0X74,0X6D,0X6C,0X3E};

#endif
 
//static const char UpdateServerIndex[] PROGMEM =
//  R"(<html charset="UTF-8">
//     <style type='text/css'>
//        body {background-color:#ddd;}
//     </style>
//     <body>
//     <h1>DSMR-logger Flash utility</h1>
//     <form method='POST' action='?cmd=0' enctype='multipart/form-data'>
//          Selecteer een "<b>.ino.bin</b>" bestand<br/>
//          <input type='file' accept='ino.bin' name='firmware'>
//          <input type='submit' value='Flash Firmware'>
//      </form>
//      <form method='POST' action='?cmd=100' enctype='multipart/form-data'> 
//          Selecteer een "<b>.spiffs.bin</b>" bestand<br/>
//          <input type='file' accept='spiffs.bin' name='filesystem'>
//          <input type='submit' value='Flash Spiffs'>
//      </form>
//      <hr>
//      <br/><font color='red'>LET OP!!!</font>
//      <br/>Bij het flashen van SPIFFS raakt u de RING-bestanden kwijt. 
//      <br/>Maak daarom eerst een kopie van deze bestanden (via bestandsbeheer) 
//      en zet de data-bestanden na het flashen van SPIFFS weer terug.
//      <hr>
//      <br/>
//      <br/>Wacht nog <span style='font-size: 1.3em;' id="waitSeconds">120</span> seconden ..
//      <br>Als het lijkt of er niets gebeurd, wacht dan tot de teller
//           op 'nul' staat en klik daarna <span style='font-size:1.3em;'><b><a href="/">hier</a></b></span>!
//     </body>
//     <script>
//         var s = document.getElementById("waitSeconds").textContent;
//         var countdown = setInterval(function() {
//           s--;
//           document.getElementById('waitSeconds').textContent = s;
//           if (s <= 0) {
//              clearInterval(countdown);
//              window.location.assign("/")
//           }
//         }, 1000);
//     </script>
//     </html>)";

//static const char UpdateServerSuccess[] PROGMEM = 
//  R"(<html charset="UTF-8">
//     <style type='text/css'>
//        body {background-color:#ddd;}
//     </style>
//     <body>
//     <h1>DSMR-logger Flash utility</h1>
//     <br/>
//     <h2>Update successful!</h2>
//     <br/>
//     <br/>Wait for the DSMR-logger to reboot and start the HTTP server
//     <br/>
//     <br>
//     <br/>Wacht nog <span style='font-size: 1.3em;' id="waitSeconds">60</span> seconden ..
//     <br/>Als het lijkt of er niets gebeurd, wacht dan tot de teller
//          op 'nul' staat en klik daarna <span style='font-size:1.3em;'><b><a href="/">hier</a></b></span>!
//     </body>
//     <script>
//         var s = document.getElementById("waitSeconds").textContent;
//         var countdown = setInterval(function() {
//           s--;
//           document.getElementById('waitSeconds').textContent = s;
//           if (s <= 0) {
//              clearInterval(countdown);
//              window.location.assign("/")
//           }
//         }, 1000);
//     </script>
//     </html>)";
