// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include <ray/mstream.h>

_NAME_BEGIN

MemoryBuf::MemoryBuf() noexcept
	: _isMappinged(false)
	, _next(0)
{
}

MemoryBuf::~MemoryBuf() noexcept
{
}

bool
MemoryBuf::open(ios_base::openmode mode) noexcept
{
	_next = 0;
	_isMappinged = false;
	return true;
}

bool
MemoryBuf::close() noexcept
{
	_data.clear();
	return true;
}

streamsize
MemoryBuf::read(char* src, std::streamsize cnt) noexcept
{
	if (_data.size() < _next + cnt)
	{
		cnt = _data.size() - _next;
		if (cnt == 0)
			return 0;
	}

	std::memcpy(src, _data.data() + _next, cnt);
	_next += cnt;

	return cnt;
}

streamsize
MemoryBuf::write(const char* src, std::streamsize cnt) noexcept
{
	if (_data.size() < _next + cnt)
		_data.resize(_next + cnt);

	std::memcpy(_data.data() + _next, src, cnt);
	_next += cnt;
	return cnt;
}

streamoff
MemoryBuf::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
{
	assert(dir == ios_base::beg || dir == ios_base::cur || dir == ios_base::end);

	if (dir == ios_base::beg)
	{
		_next = pos;
		return pos;
	}
	else if (dir == ios_base::cur)
	{
		_next = _next + pos;
		if (_next > _data.size())
		{
			pos = _data.size() - _next;
			_next = _data.size();
		}

		return pos;
	}
	else if (dir == ios_base::end)
	{
		std::size_t size = _data.size();
		pos = size + pos;
		if (pos > size)
			_next = size;
		else
			_next = pos;
		return pos;
	}

	return 0;
}

streamoff
MemoryBuf::tellg() noexcept
{
	return _next;
}

streamsize
MemoryBuf::size() const noexcept
{
	return _data.size();
}

bool
MemoryBuf::is_open() const noexcept
{
	return !_data.empty();
}

int
MemoryBuf::flush() noexcept
{
	return 0;
}

void
MemoryBuf::resize(streamsize size) noexcept
{
	_data.resize(size);
}

char*
MemoryBuf::map() noexcept
{
	assert(!_isMappinged);
	if (_data.size())
	{
		_isMappinged = true;
		return _data.data();
	}

	return nullptr;
}

void
MemoryBuf::unmap() noexcept
{
	assert(_isMappinged);
	_isMappinged = false;
}

bool
MemoryBuf::isMapping() const noexcept
{
	return _isMappinged;
}

MemoryReader::MemoryReader() noexcept
	: StreamReader(&_buf)
{
}

MemoryReader::~MemoryReader() noexcept
{
}

void
MemoryReader::resize(streamsize size) noexcept
{
	_buf.resize(size);
}

char*
MemoryReader::map() noexcept
{
	return _buf.map();
}

void
MemoryReader::unmap() noexcept
{
	return _buf.unmap();
}

bool
MemoryReader::isMapping() const noexcept
{
	return _buf.isMapping();
}

MemoryWrite::MemoryWrite() noexcept
	: StreamWrite(&_buf)
{
}

MemoryWrite::~MemoryWrite() noexcept
{
}

streamsize
MemoryWrite::size() const noexcept
{
	return _buf.size();
}

void
MemoryWrite::resize(streamsize size) noexcept
{
	_buf.resize(size);
}

char*
MemoryWrite::map() noexcept
{
	return _buf.map();
}

void
MemoryWrite::unmap() noexcept
{
	return _buf.unmap();
}

bool
MemoryWrite::isMapping() const noexcept
{
	return _buf.isMapping();
}

MemoryStream::MemoryStream() noexcept
	: Stream(&_buf)
{
}

MemoryStream::~MemoryStream() noexcept
{
}

void
MemoryStream::resize(streamsize size) noexcept
{
	_buf.resize(size);
}

char*
MemoryStream::map() noexcept
{
	return _buf.map();
}

void
MemoryStream::unmap() noexcept
{
	_buf.unmap();
}

bool
MemoryStream::isMapping() const noexcept
{
	return _buf.isMapping();
}

_NAME_END