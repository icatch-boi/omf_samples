//
// Created by jg.wang on 2020/6/8.
//
#pragma once
#ifndef STREAM_OMFSHMMAP_H
#define STREAM_OMFSHMMAP_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

///
EXTERNC void* omfShmMap(int*psize);

#endif //STREAM_OMFSHMMAP_H
