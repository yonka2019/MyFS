#include "myfs.h"

const char *MyFs::MYFS_MAGIC = "MYFS";

MyFs::MyFs(BlockDeviceSimulator *blkdevsim_):blkdevsim(blkdevsim_) {

	struct myfs_header header;
	_current = 0;

	blkdevsim->read(0, sizeof(header), (char *)&header);
	if (strncmp(header.magic, MYFS_MAGIC, sizeof(header.magic)) != 0 || // myfs instance NOT exist
	    (header.version != CURR_VERSION)) {
		std::cout << "Did not find myfs instance on blkdev" << std::endl;
		std::cout << "Creating..." << std::endl;

		format();
		std::cout << "Finished!" << std::endl;
	}
	else // myfs instance exist ==> restore data
	{
		std::string line, stringBuilder = "";
		std::fstream myfile (blkdevsim->getName());

		if (myfile.is_open()) // only if file opened successfully
		{
			while (getline(myfile, line))
			{
				stringBuilder += line;
			}

			myfile.close();

			blkdevsim->write(0, stringBuilder.size(), stringBuilder.c_str());
			_current = stringBuilder.size(); // not += because we are restoring all data (include MYFS header)
		}
	}
}

void MyFs::format() {

	struct myfs_header header;
	strncpy(header.magic, MYFS_MAGIC, sizeof(header.magic));
	header.version = CURR_VERSION;
	blkdevsim->write(_current, sizeof(header), (const char*)&header);

	_current += sizeof(header);
}

void MyFs::create_file(const std::string path_str, const bool directory) {
	
	std::string BDContent = getBlockDeviceContent();

	if (BDContent.find(path_str) == std::string::npos) // check if file already exist (not exist)
	{
		std::string fStr = formatStr(path_str, "");
		std::size_t fSize = fStr.size();

		blkdevsim->write(_current, fSize, fStr.c_str());
		_current += fSize;
	}
	// if exist -> do nothing =D
}

std::string MyFs::get_content(const std::string path_str) const {

	std::string BDContent = getBlockDeviceContent();
	std::regex rContent("\\{\\[" + path_str + "\\]\\[\\d+\\]\\[(.*?)\n?]\\}");
	std::smatch matches;

	if (std::regex_search(BDContent, matches, rContent))
	{
		return matches.str(1);
	}
	else
	{
		return "";
	}
}

void MyFs::set_content(const std::string path_str, const std::string content) {

	std::string BDContent = getBlockDeviceContent(); // extracts block device content
	std::string fStr = formatStr(path_str, content);
	std::size_t fSize = fStr.size();

	if (BDContent.find(path_str) == std::string::npos) // check if file already exist (not exist)
	{
		blkdevsim->write(_current, fSize, fStr.c_str()); // adds new file into BD (creates new file)
		_current += fSize;
	}
	else // insertting new content into the existing file (file exist)
	{ // '{[path_str][(size)(][)(content)(]})'

		std::string strPart = "{[" + path_str + "][" + std::to_string(content.length());

		std::regex rContent("\\{\\[" + path_str + "\\]\\[\\d+(\\]\\[).*?\n?(\\]\\})"); // regex to catch content according the file name
		std::string newContent = std::regex_replace(BDContent, rContent, strPart + "$1" + content + "$2"); //changes block device content

		clearBlockDeviceContent(); // clears up the block device content
		blkdevsim->write(0, newContent.size(), newContent.c_str()); // rewrites block device content

		_current = newContent.size(); // updates the current address of the block device
	}
}

MyFs::dir_list MyFs::list_dir(const std::string path_str) const {

	dir_list ans;
	dir_list_entry file;
	std::string BDContent = getBlockDeviceContent();
	std::regex rFile("\\{\\[(.+?)\\]\\[(\\d+?)\\]\\[(.*?)\n?]\\}");

    std::sregex_iterator iter(BDContent.begin(), BDContent.end(), rFile);
    std::sregex_iterator end;

    while(iter != end)
    {
		file.name = (*iter)[1];
		file.file_size = std::stoi(((*iter)[2]).str());
		file.is_dir = false;

		ans.push_back(file);
        iter++;
    }
	return ans;
}

std::string MyFs::formatStr(const std::string path_str, const std::string content)
{
	return ("{[" + path_str + "][" + std::to_string(content.size()) + "][" + content + "]}");
}

void MyFs::saveBlockDeviceContent() const
{
	std::string BDContent = getBlockDeviceContent();
	std::ofstream myfile (blkdevsim->getName());

	if (myfile.is_open()) // continue only if the file successfully opened
	{
		myfile << BDContent;
		myfile.close();
	}
}

std::string MyFs::getBlockDeviceContent() const
{
	char content[345];

	std::fill_n(content, 345, '\0');
	blkdevsim->read(0, _current, content);

	return std::string(content);
}

void MyFs::clearBlockDeviceContent()
{
	std::string nulls(_current, '\0'); // create string with length '_current' fulled with '\0'[NULL] symbols

	blkdevsim->write(0, _current, nulls.c_str()); // write into BD
}

void MyFs::remove_file(const std::string path_str)
{
	std::string BDContent = getBlockDeviceContent();
	std::regex rPathFile("\\{\\[" + path_str + "\\]\\[(\\d+?)\\]\\[(.*?)\n?]\\}");

	std::string removedFile = std::regex_replace(BDContent, rPathFile, ""); 

	clearBlockDeviceContent();
	blkdevsim->write(0, removedFile.size(), removedFile.c_str());

	_current = removedFile.size(); // update current address 
}