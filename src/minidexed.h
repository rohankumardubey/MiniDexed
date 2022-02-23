//
// minidexed.h
//
#ifndef _minidexed_h
#define _minidexed_h

#include <synth_dexed.h>
#include <stdint.h>
#include <math.h>
#include <circle/interrupt.h>
#include <circle/i2cmaster.h>
#include <circle/usb/usbmidi.h>
#include <circle/serial.h>
#include <circle/types.h>
#include <circle/pwmsoundbasedevice.h>
#include <circle/i2ssoundbasedevice.h>
#include <circle/hdmisoundbasedevice.h>
#include "pckeyboard.h"

#define SAMPLE_RATE	48000

#define CHUNK_SIZE	2048
#define CHUNK_SIZE_HDMI	(384 * 10)

#define DAC_I2C_ADDRESS	0		// I2C slave address of the DAC (0 for auto probing)

class CMiniDexed : public Dexed
{
  public:
    CMiniDexed(uint8_t max_notes, uint16_t sample_rate, CInterruptSystem *pInterrupt)
:   Dexed(max_notes,(int)sample_rate),
    m_pMIDIDevice (0),
    m_PCKeyboard (this),
    m_Serial (pInterrupt, TRUE),
    m_bUseSerial (FALSE),
    m_nSerialState (0)
    {
      s_pThis = this;
    };

    virtual bool Initialize (void);
    void Process(boolean bPlugAndPlayUpdated);

  protected:
    static void MIDIPacketHandler (unsigned nCable, u8 *pPacket, unsigned nLength);
    static void KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6]);
    static void USBDeviceRemovedHandler (CDevice *pDevice, void *pContext);
    CUSBMIDIDevice     * volatile m_pMIDIDevice;
    CPCKeyboard        m_PCKeyboard;
    CSerialDevice m_Serial;
    boolean m_bUseSerial;
    unsigned m_nSerialState;
    u8 m_SerialMessage[3];

    static CMiniDexed *s_pThis;
};

class CMiniDexedPWM : public CMiniDexed, public CPWMSoundBaseDevice
{
  public:
    CMiniDexedPWM(uint8_t max_notes, uint16_t sample_rate, CInterruptSystem *pInterrupt)
:   CMiniDexed(max_notes,(int)sample_rate, pInterrupt),
    CPWMSoundBaseDevice (pInterrupt, sample_rate, CHUNK_SIZE)
    {
    }

    bool Initialize (void);
    unsigned GetChunk (u32 *pBuffer, unsigned nChunkSize);
};

class CMiniDexedI2S : public CMiniDexed, public CI2SSoundBaseDevice
{
  public:
    CMiniDexedI2S(uint8_t max_notes, uint16_t sample_rate, CInterruptSystem *pInterrupt, CI2CMaster *pI2CMaster)
:   CMiniDexed(max_notes,(int)sample_rate, pInterrupt),
    CI2SSoundBaseDevice (pInterrupt, sample_rate, CHUNK_SIZE, FALSE, pI2CMaster, DAC_I2C_ADDRESS)
    {
    }

    bool Initialize (void);
    unsigned GetChunk (u32 *pBuffer, unsigned nChunkSize);
};

class CMiniDexedHDMI : public CMiniDexed, public CHDMISoundBaseDevice
{
  public:
    CMiniDexedHDMI(uint8_t max_notes, uint16_t sample_rate, CInterruptSystem *pInterrupt)
:   CMiniDexed(max_notes,(int)sample_rate, pInterrupt),
    CHDMISoundBaseDevice (pInterrupt, sample_rate, CHUNK_SIZE_HDMI)
    {
    }

    bool Initialize (void);
    unsigned GetChunk (u32 *pBuffer, unsigned nChunkSize);
};

#endif