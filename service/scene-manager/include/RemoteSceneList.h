//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef SM_REMOTE_SCENELIST_H_
#define SM_REMOTE_SCENELIST_H_

#include <memory>
#include <vector>
#include <functional>
#include <mutex>

#include "RemoteSceneCollection.h"
#include "RCSRemoteResourceObject.h"

namespace OIC
{
    namespace Service
    {

        class SceneListResourceRequestor;

        class RemoteSceneList
            : public std::enable_shared_from_this< RemoteSceneList >
        {
            public:
                typedef std::unique_ptr< RemoteSceneList > Ptr;

                typedef std::function< void(RemoteSceneList::Ptr, int eCode) >
                    CreateInstanceCallback;

                typedef std::function< void(RemoteSceneCollection::Ptr, int eCode) >
                    AddNewSceneCollectionCallback;

                typedef std::function< void(int eCode) > SetNameCallback;

            public:
                ~RemoteSceneList() = default;

                static void createInstance(
                    RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback);

                void addNewSceneCollection(AddNewSceneCollectionCallback);
                void removeSceneCollection(RemoteSceneCollection::Ptr);

                std::vector< RemoteSceneCollection::Ptr > getRemoteSceneCollections() const;

                void setName(const std::string &name, SetNameCallback);
                std::string getName() const;

            private:
                class GetResponseHandler
                {
                    public:
                        typedef std::shared_ptr< GetResponseHandler > Ptr;

                        typedef std::function< void(int eCode) >
                            GetCallback;

                        GetResponseHandler(std::shared_ptr< RemoteSceneList >);
                        ~GetResponseHandler() = default;

                        int m_numOfCollections;
                        int m_respondedCollections;
                        int m_errorCode;
                        std::weak_ptr< RemoteSceneList > m_owner;
                        GetCallback m_cb;

                        void startGetResponseHandler(const std::string &host, GetCallback cb);

                        void onGetListAttrs(const RCSRepresentation &, int, const std::string &);
                        
                        void onGetCollectionAttrs(const RCSRepresentation &, int eCode,
                            RemoteSceneCollection::Ptr, const std::string &);
                };
                
            private:
                RemoteSceneList(std::shared_ptr< SceneListResourceRequestor >);

                static void onInstanceCreated(const RCSRepresentation &, int, const std::string &,
                    std::shared_ptr< SceneListResourceRequestor >, const CreateInstanceCallback &);

                static RemoteSceneList::Ptr buildSceneList(std::shared_ptr< SceneListResourceRequestor >,
                    const RCSResourceAttributes &);

                RemoteSceneCollection::Ptr createRemoteSceneCollectionInstance(
                    const std::string &link, const std::string &id, const std::string &name);

                std::shared_ptr< SceneListResourceRequestor > getListResourceRequestor() const;
                
                void setGetResponseHandler(const std::string &, GetResponseHandler::GetCallback);

                std::vector<std::pair<RCSResourceAttributes, std::vector<RCSResourceAttributes>>>
                    parseSceneListFromAttributes(const RCSResourceAttributes &);

                std::vector<RCSResourceAttributes> getChildrenAttributes(
                    const RCSResourceAttributes &) const;

                void onSceneCollectionCreated(
                    const std::string &link, const std::string &id, const std::string &name,
                    int, const AddNewSceneCollectionCallback &);

                void onNameSet(int, const std::string &, const SetNameCallback &);

            private:
                std::string m_name;
                std::mutex m_collectionLock;
                std::vector< RemoteSceneCollection::Ptr > m_remoteSceneCollections;
                std::shared_ptr< SceneListResourceRequestor > m_requestorPtr;

                GetResponseHandler::Ptr m_getResponseHandler;

                friend class GetResponseHandler;
        };

    }
}

#endif /* SM_REMOTE_SCENELIST_H_ */
