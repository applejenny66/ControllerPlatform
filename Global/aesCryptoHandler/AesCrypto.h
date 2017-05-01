#pragma once

#include <string>
#include <unistd.h>

/**
 * A helper class for doing AES 256 bit, CBC mode encryption/decryption.
 * 
 * This class relies on crypto++, which can be installed through apt on Debian:
 *     sudo apt-get -y install libcrypto++-dev
 * Linking option: -lcryptopp
 */
class AesCrypto
{
public:
	/**
	 * Length of initial vector in bytes.
	 * Value should be same as block size which is always 128 bits in AES
	 */
	static const int IvLength = 16;

	/**
	 * Length of key in bytes.
	 */
	static const int KeyLength = 32; 

	/**
	 * Instantiate with a key used for encryption and decryption.
	 */
	AesCrypto(uint8_t *key);

	std::string encrypt(const std::string plaintext, const uint8_t *iv);
	std::string decrypt(const uint8_t* ciphertext, const int textLength, const uint8_t *iv);

	/**
	 * A handy function for generating random IV or (probably not) key.
	 */
	static void getRandomBytes(uint8_t *outBuf, const int bufLen);

private:
	uint8_t *key;
};

/** example of usage, adapted from official Crypto++ example using CBC mode

#include <string>
void cryptoppTest() {
	// Byte length of key must be 32 bytes (not including teminating null)
	std::string key = "12345678901234567890123456789012";
	uint8_t *key = (uint8_t *)key.c_str();

	// Initial vector can be a array filled with zero or constant value
	// but it raises security concern, not recommended
	uint8_t *iv = new uint8_t[AesCrypto::IvLength];

	auto aes = AesCrypto(key);
	AesCrypto::getRandomBytes(iv, AesCrypto::IvLength);

	//
	// String and Sink setup
	//
	std::string plaintext = "{\"verb\":\"open\",\"subject\":\"yooooo\",\"object\":\"001\"}";
	std::string ciphertext;
	std::string decryptedtext;

	//
	// Dump Plain Text
	//
	std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
	std::cout << plaintext;
	std::cout << std::endl << std::endl;

	//
	// Create Cipher Text
	//
	ciphertext = std::string(aes.encrypt(plaintext, iv));

	//
	// Dump Cipher Text
	//
	std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

	for (unsigned int i = 0; i < ciphertext.size(); i++) {
	std::cout << "0x" << std::hex << (0xFF & static_cast<uint8_t>(ciphertext[i])) << " ";
	}

	std::cout << std::endl << std::endl;

	//
	// Decrypt
	//
	decryptedtext = std::string(aes.decrypt((unsigned char*)(ciphertext.c_str()),
	ciphertext.size(), iv));


	//
	// Dump Decrypted Text
	//
	std::cout << "Decrypted Text: " << std::endl;
	std::cout << decryptedtext;
	std::cout << std::endl << std::endl;
}
*/