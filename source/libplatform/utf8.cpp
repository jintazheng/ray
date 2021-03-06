﻿// +----------------------------------------------------------------------
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
#include <ray/utf8.h>
#include <ray/string.h>
#include <iconv.h>

_NAME_BEGIN

std::size_t utf8_to_gbk(char* dest, std::size_t maxLength, const char* data, std::size_t size)
{
	char inbuf[PATHLIMIT + 1] = { 0 };
	char outbuf[PATHLIMIT + 1] = { 0 };
	char *in = inbuf;
	char *out = outbuf;
	std::size_t in_size = (size_t)size;
	std::size_t out_size = (size_t)PATHLIMIT;

	memcpy(in, data, size);

	iconv_t ic = iconv_open("GBK", "UTF-8");
	iconv(ic, &in, &in_size, &out, &out_size);
	iconv_close(ic);

	out_size = PATHLIMIT - out_size;
	memcpy(dest, outbuf, out_size + 1);

	return out_size;
}

std::size_t utf8_to_utf16(const char* src, wchar_t* dest, std::size_t size)
{
	return utf8_to_utf16(src, size, dest, size);
}

std::size_t utf8_to_utf16(const char* data, std::size_t dataSize, wchar_t* dest, std::size_t maxLength)
{
#if defined(_linux_)
	char inbuf[PATHLIMIT + 1] = { 0 };
	char outbuf[PATHLIMIT + 1] = { 0 };
	char *in = inbuf;
	char *out = outbuf;
	std::size_t in_size = (size_t)size;
	std::size_t out_size = (size_t)PATHLIMIT;

	memcpy(in, data, size);

	utf8_to_gbk(outbuf, PATHLIMIT, inbuf, size);

	in = outbuf;
	out = inbuf;

	iconv_t ic = iconv_open("WCHAR_T", "UTF-8");
	iconv(ic, &in, &in_size, &out, &out_size);
	iconv_close(ic);

	out_size = PATHLIMIT - out_size;
	memcpy(dest, inbuf, out_size);

	return out_size;
#else
	int size = MultiByteToWideChar(CP_UTF8, 0, data, -1, 0, 0) + 1;
	assert(size > 1);

	if (size <= 1)
		return 0;

	if (size >= maxLength)
		return 0;

	int len = MultiByteToWideChar(CP_UTF8, 0, data, -1, dest, size);
	assert(len);

	return len;
#endif
}

std::size_t utf16_to_utf8(const wchar_t * unicode, char* szUtf8, std::size_t max_length)
{
	return utf16_to_utf8(unicode, max_length, szUtf8, max_length);
}

std::size_t utf16_to_utf8(const wchar_t * src, std::size_t multiByteStr, char* dest, std::size_t max_length)
{
#if defined(_linux_)
	::setlocale(LC_ALL, "zh_CN.UTF8");
	int size = wcstombs(NULL, wcharStr, 0);
	str = new char[size + 1];
	wcstombs(str, wcharStr, size);
	str[size] = '\0';
	return str;
#else
	int len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL) + 1;
	assert(len > 1);

	if (len <= 1)
		return 0;

	if (len >= max_length)
		return 0;

	int size = WideCharToMultiByte(CP_UTF8, 0, src, -1, dest, len + 1, NULL, NULL);
	assert(size);

	return size;
#endif
}

std::size_t acp_to_utf16(const char* src, wchar_t* dest, std::size_t max_length)
{
	return acp_to_utf16(src, max_length, dest, max_length);
}

std::size_t acp_to_utf16(const char* pszSrc, std::size_t multiByteStr, wchar_t* dest, std::size_t max_length)
{
#if defined(_linux_)
	::setlocale(LC_ALL, "");
	int size = mbstowcs(NULL, pszSrc, 0);
	if (size <= 0)
		return false;
	if (max_length < size)
		return false;

	size = mbstowcs(dest, pszSrc, size + 1);
	dest[size] = 0;
	return true;
#else
	int size = MultiByteToWideChar(CP_THREAD_ACP, 0, pszSrc, -1, 0, 0) + 1;
	assert(size > 1);

	if (size <= 1)
		return 0;

	if (size >= max_length)
		return 0;

	int len = MultiByteToWideChar(CP_THREAD_ACP, 0, pszSrc, -1, dest, size);
	assert(len);

	return len;
#endif
}

std::size_t utf16_to_acp(const wchar_t* src, std::size_t max_length, char* dest, std::size_t multiByteStr)
{
#if defined(_linux_)
	::setlocale(LC_ALL, "zh_CN.UTF8");
	int size = wcstombs(NULL, pszSrc, 0);
	if (size <= 0)
		return false;
	if (max_length < size)
		return false;

	size = wcstombs(dest, pszSrc, size + 1);
	dest[size] = 0;
	return true;
#else
	int len = WideCharToMultiByte(CP_THREAD_ACP, 0, src, -1, NULL, 0, NULL, NULL) + 1;
	assert(len > 1);

	if (len <= 1)
		return 0;

	if (len >= max_length)
		return 0;

	len = WideCharToMultiByte(CP_THREAD_ACP, 0, src, -1, dest, len, NULL, NULL);
	assert(len);

	return len;
#endif
}

_NAME_END