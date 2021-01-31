using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BlGame.Resource
{
    public enum RequestType
    {
        LOAD,
        UNLOAD,
        LOADLEVEL,
        UNLOADLEVEL,
    }

    class Request
    {
        internal string mFileName;              //请求资源相对Assets/完整路径名称
        internal ResourceType mResourceType;
        internal ResourcesManager.HandleFinishLoad mHandle;
        internal ResourcesManager.HandleFinishLoadLevel mHandleLevel;
        internal ResourcesManager.HandleFinishUnLoadLevel mHandleUnloadLevel;
        internal RequestType mRequestType;
        internal ResourceAsyncOperation mResourceAsyncOperation;

        //internal Request(string path, ResourceType resourceType, ResourcesManager.HandleFinishLoad handle, RequestType requestType, ResourceAsyncOperation operation)
        //{
        //    mPath = path;
        //    mResourceType = resourceType;
        //    mHandle = handle;
        //    mRequestType = requestType;
        //    mResourceAsyncOperation = operation;
        //}

        internal Request(string fileName, ResourceType resourceType, ResourcesManager.HandleFinishLoad handle, RequestType requestType, ResourceAsyncOperation operation)
        {
            mFileName = fileName;
            mResourceType = resourceType;
            mHandle = handle;
            mRequestType = requestType;
            mResourceAsyncOperation = operation;
        }

        internal Request(string fileName, ResourceType resourceType, ResourcesManager.HandleFinishLoadLevel handle, RequestType requestType, ResourceAsyncOperation operation)
        {
            mFileName = fileName;
            mResourceType = resourceType;
            mHandleLevel = handle;
            mRequestType = requestType;
            mResourceAsyncOperation = operation;
        }

        //internal Request(string fileName, string archiveName, ResourceType resourceType, ResourcesManager.HandleFinishLoadLevel handle, RequestType requestType, ResourceAsyncOperation operation)
        //{
        //    mFileName = fileName;
        //    mArchiveName = archiveName;
        //    mResourceType = resourceType;
        //    mHandleLevel = handle;
        //    mRequestType = requestType;
        //    mResourceAsyncOperation = operation;
        //}

        //internal Request(string path, RequestType requestType)
        //{
        //    mPath = path;
        //    mRequestType = requestType;
        //}

        //internal Request(string fileName, string archiveName, RequestType requestType)
        //{
        //    mFileName = fileName;
        //    mArchiveName = archiveName;
        //    mRequestType = requestType;
        //}

        //internal Request(string fileName, string archiveName, ResourcesManager.HandleFinishUnLoadLevel handle, RequestType requestType)
        //{
        //    mFileName = fileName;
        //    mArchiveName = archiveName;
        //    mHandleUnloadLevel = handle;
        //    mRequestType = requestType;
        //}
    }
}