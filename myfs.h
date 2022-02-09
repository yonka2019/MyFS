#ifndef __MYFS_H__
#define __MYFS_H__

#include "blkdev.h"
#include <memory>
#include <vector>
#include <stdint.h>
#include <regex>
#include <string.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <typeinfo>
#include <fstream>

class MyFs {
public:
	MyFs(BlockDeviceSimulator *blkdevsim_);

	/**
	 * dir_list_entry struct
	 * This struct is used by list_dir method to return directory entry
	 * information.
	 */
	struct dir_list_entry {
		/**
		 * The directory entry name
		 */
		std::string name;

		/**
		 * whether the entry is a file or a directory
		 */
		bool is_dir;

		/**
		 * File size
		 */
		int file_size;
	};
	typedef std::vector<struct dir_list_entry> dir_list;

	/**
	 * format method
	 * This function discards the current content in the blockdevice and
	 * create a fresh new MYFS instance in the blockdevice.
	 */
	void format();

	/**
	 * create_file method
	 * Creates a new file in the required path.
	 * @param path_str the file path (e.g. "/newfile")
	 * @param directory boolean indicating whether this is a file or directory
	 */
	void create_file(const std::string path_str, const bool directory);

	/**
	 * get_content method
	 * Returns the whole content of the file indicated by path_str param.
	 * Note: this method assumes path_str refers to a file and not a
	 * directory.
	 * @param path_str the file path (e.g. "/somefile")
	 * @return the content of the file
	 */
	std::string get_content(const std::string path_str) const;

	/**
	 * set_content method
	 * Sets the whole content of the file indicated by path_str param.
	 * Note: this method assumes path_str refers to a file and not a
	 * directory.
	 * @param path_str the file path (e.g. "/somefile")
	 * @param content the file content string
	 */
	void set_content(const std::string path_str, const std::string content);

	/**
	 * list_dir method
	 * Returns a list of a files in a directory.
	 * Note: this method assumes path_str refers to a directory and not a
	 * file.
	 * @param path_str the file path (e.g. "/somedir")
	 * @return a vector of dir_list_entry structures, one for each file in
	 *	the directory.
	 */
	dir_list list_dir(const std::string path_str) const;
	
	/**
	 * @brief formats and returns string according the given parameters '{[path_str][content.size][content]}'
	 * 
	 * @param path_str path to the file
	 * @param content content of the file
	 * @return formatted string accoriding the given parameters
	 */
	std::string formatStr(const std::string path_str, const std::string content);

	/**
	 * @brief gets the full block device content
	 * 
	 * @return full block device content
	 */
	std::string getBlockDeviceContent() const;

	/**
	 * @brief clearing the whole content of the block device that we use
	 * 
	 */
	void clearBlockDeviceContent();

	/**
	 * @brief removes the file from the block device
	 * 
	 * @param path_str path of the file which we want to delete
	 */
	void remove_file(const std::string path_str);
	
	/**
	 * @brief saves the content of the current block device into file with the block device names
	 * 
	 */
	void saveBlockDeviceContent() const;

	/**
	 * @brief current address of the memory
	 * 
	 */
	int _current;

private:

	/**
	 * This struct represents the first bytes of a myfs filesystem.
	 * It holds some magic characters and a number indicating the version.
	 * Upon class construction, the magic and the header are tested - if
	 * they both exist than the file is assumed to contain a valid myfs
	 * instance. Otherwise, the blockdevice is formated and a new instance is
	 * created.
	 */
	struct myfs_header {
		char magic[4];
		uint8_t version;
	};

	BlockDeviceSimulator *blkdevsim;

	static const uint8_t CURR_VERSION = 0x03;
	static const char *MYFS_MAGIC;
};

#endif // __MYFS_H__
