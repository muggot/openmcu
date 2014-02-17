//	NodeRelease.h
//	-------------
//	Copyright 1999, Be Incorporated.   All Rights Reserved.
//	This file may be used under the terms of the Be Sample Code License.

#if !defined(NodeRelease_h)
#define NodeRelease_h

#include <MediaRoster.h>

namespace BPrivate {
	class StNodeRelease {
		media_node & n;
		bool r;
	public:
		StNodeRelease(media_node & nod, bool rel = true) : n(nod), r(rel) { }
		~StNodeRelease() { if (r) BMediaRoster::Roster()->ReleaseNode(n); }
		void SetRelease(bool rel) { r = rel; }
	};
};
using namespace BPrivate;

#endif	//	NodeRelease_h
