#pragma once
#include "_typedef.h"

namespace omf{
	class File{//:public object {
	public:
		enum Position {
			FRONT = 0,
			CURRENT = 1,
			END = 2,
		};
		enum Attr {
			ATTR_MODIFY_NAME  = (1 << 0),
			ATTR_MODIFY_ATTR  = (1 << 1),
			ATTR_MODIFY_SIZE  = (1 << 2),
			ATTR_MODIFY_START = (1 << 3),
		};
	public:
		File(const char* fn, const char* mode="rwbc");
		virtual ~File();
		//virtual void operator>>(ostream&os) const OVERRIDE;
	protected:
		void* _fd;
		const char* _err;
	public:
		//using object::Name;
		//virtual bool Name(const char* name);
		const char* Error() const;
		uint32 Write(const void* buf, uint32 size);
		uint32 Read(void* buf, uint32 size = 0x7fffffff);
		bool Seek(int64 offset, Position pos);
		int64 Tell() const;
		int64 Size() const;

	public:
		static void* Open(const char* fn, const char* mode);
		static void Close(void* fd);
		static uint32 Write(void* fd, const void* data, uint32 size);
		static uint32 Read(void* fd, void* data, uint32 size);
		static bool Seek(void* fd, int64 offset, Position pos);
		static int64 Tell(void* fd);
		static int64 Size(void* fd);
		static bool Flush(void* fd);
		static bool Sync(void* fd);
		static bool Sync(int fd);
		static uint32 Concate(void* srcfd,void* dstfd,int opt);
		static uint32 GetFileId(void* fd);
		static uint32 Write(const char* fn, const void* data, uint32 size);
		static uint32 Read(const char* fn, void* data, uint32 size);
		static bool Rename(const char* fnold, const char* fnnew);
		static bool Delete(const char* fn);
		static uint32 AttrSet(const char* pathname, const char* pnewName, int64 attr, uint32 option);
	};
}
