pthread_mutex_t& PSoundChannelCoreAudio::GetReadMuteMutex(){
	static pthread_mutex_t isReadMute_Mutex = PTHREAD_MUTEX_INITIALIZER;
	return isReadMute_Mutex;
}


pthread_mutex_t& PSoundChannelCoreAudio::GetWriteMuteMutex(){
	static pthread_mutex_t isWriteMute_Mutex = PTHREAD_MUTEX_INITIALIZER;
	return isWriteMute_Mutex;
}

BOOL& PSoundChannelCoreAudio::GetReadMute(){
	static BOOL isReadMute(FALSE);
	return isReadMute;
}

BOOL& PSoundChannelCoreAudio::GetWriteMute(){
	static BOOL isWriteMute(FALSE);
	return isWriteMute;
}

pthread_mutex_t& PSoundChannelCoreAudio::GetIsMuteMutex(){
	if(direction == Recorder){
		return GetReadMuteMutex();
	} else {
		return GetWriteMuteMutex();
	}
}

BOOL& PSoundChannelCoreAudio::isMute(){
	if(direction == Recorder){
		return GetReadMute();
	} else {
		return GetWriteMute();
	}
}




