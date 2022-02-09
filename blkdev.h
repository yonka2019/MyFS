#ifndef __BLKDEVSIM__H__
#define __BLKDEVSIM__H__

#include <string>

class BlockDeviceSimulator {
public:
	BlockDeviceSimulator(std::string fname);
	~BlockDeviceSimulator();

	void read(int addr, int size, char *ans);
	void write(int addr, int size, const char *data);
	/**
	 * @brief Gets the name of the block device
	 * 
	 * @return Name of the block device 
	 */
	std::string getName() const;

	static const int DEVICE_SIZE = 1024 * 1024;

private:
	int fd;
	unsigned char *filemap;
	std::string name;
};

#endif // __BLKDEVSIM__H__
