BIN_DIR = ./bin

MYFS_HEADERS = blkdev.h myfs.h
MYFS_SRC_FILES = blkdev.cpp myfs.cpp

MYFS_MAIN_SRC = $(MYFS_SRC_FILES) myfs_main.cpp

all: ${BIN_DIR}/myfs

${BIN_DIR}/myfs: $(MYFS_MAIN_SRC) $(MYFS_HEADERS) ${BIN_DIR}/.exist
	g++ ${MYFS_MAIN_SRC}  -o ${BIN_DIR}/myfs -g -Wall

${BIN_DIR}/.exist:
	mkdir ${BIN_DIR}
	touch ${BIN_DIR}/.exist

clean:
	rm  -f ${BIN_DIR}/myfs
