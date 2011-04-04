/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "Object/Platform/StableHeaders.h"

#include "Object/ComponentFactory.h"
#include "Object/ComponentFactoryManager.h"
#include "Object/ComponentTemplate.h"

namespace Diversia
{
namespace ObjectSystem
{
//------------------------------------------------------------------------------

ComponentTemplate::ComponentTemplate( const String& rName, Mode mode, NetworkingType networkingType,
    ComponentType type, RakNet::RakNetGUID source, bool localOverride, 
    ObjectTemplate& rObjectTemplate ):
    mName( rName ),
    mMode( mode ),
    mNetworkingType( networkingType ),
    mType( type ),
    mSource( source == rObjectTemplate.getServerGUID() ? SERVER : CLIENT ),
    mSourceGUID( source ),
    mLocalOverride( localOverride ),
    mBroadcastingDestruction( false ),
    mObjectTemplate( rObjectTemplate ),
    mFactory( ComponentFactoryManager::getComponentFactory( type ) )
{
    // Override networking type to local.
    if( mLocalOverride || ( mMode == CLIENT && mFactory.clientOnly() ) ||
        ( mMode == SERVER && mFactory.serverOnly() ) )
    {
        mNetworkingType = LOCAL;
        mLocalOverride = true;
    }

    this->SetNetworkIDManager( &mObjectTemplate.getNetworkIDManager() );

    if( ( ( mMode == SERVER && mNetworkingType == REMOTE ) || ( mMode == CLIENT &&
        mNetworkingType == REMOTE && mSource == CLIENT ) ) && !mLocalOverride )
    {
        broadcastConstruction();
    }
}

ComponentTemplate::~ComponentTemplate()
{
    mDestructionSignal( *this );
}

String ComponentTemplate::getTypeName() const
{
    return mFactory.getTypeName();
}

void ComponentTemplate::setLocalOverride( bool localOverride )
{
    if( mLocalOverride != localOverride )
    {
        if( mMode == CLIENT && mFactory.clientOnly() && !localOverride )
            DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
                "This component can only exist on the client, cannot set local override to false.",
                "ComponentTemplate::setLocalOverride" );
        if( mMode == SERVER && mFactory.serverOnly() && !localOverride )
            DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE,
                "This component can only exist on the server, cannot set local override to false.",
                "ComponentTemplate::setLocalOverride" );

        // Prevent changing local override on components the client doesn't own.
        if( !ComponentTemplate::isCreatedByOwnGUID() )
            DIVERSIA_EXCEPT( Exception::ERR_PERMISSION_DENIED,
                "Can only set owned components to be local overridden",
                "ComponentTemplate::setLocalOverride" );

        mLocalOverride = localOverride;

        if( mLocalOverride && mNetworkingType == REMOTE )
            ComponentTemplate::setNetworkingType( LOCAL );
    }
}

void ComponentTemplate::setNetworkingType( NetworkingType type )
{
    if( mNetworkingType != type )
    {
        // Do not allow local overridden components to become remote.
        if( mLocalOverride && type == REMOTE ) return;

        mNetworkingType = type;

        setNetworkingTypeImpl( mNetworkingType );

        if( ( ( mMode == SERVER && mNetworkingType == REMOTE ) ||
            ( mMode == CLIENT && mNetworkingType == REMOTE && mSource == CLIENT ) ) )
        {
            ComponentTemplate::broadcastConstruction();
        }
        else if( mNetworkingType == LOCAL )
        {
            ComponentTemplate::broadcastDestruction();
        }
    }
}

void ComponentTemplate::destroyComponentTemplateLocally()
{
    try
    {
        // Force networking type to local so that destruction won't be broadcasted since it's a
        // local destruction. This is only done for the creator of the object because the creator
        // needs to change the permission counters.
        if( ComponentTemplate::isCreatedByOwnGUID() && mNetworkingType != LOCAL )
        {
            mNetworkingType = LOCAL;
            mLocalOverride = false;
            setNetworkingTypeImpl( mNetworkingType );
        }

        // Act as server to evade permission checks.
        mObjectTemplate.destroyComponentTemplate( getName(), ComponentTemplate::getServerGUID() );
    }
    catch ( Exception e )
    {
        DivAssert( 0, e.what() );
    }
}

void ComponentTemplate::broadcastConstruction()
{
    OLOGD << "Broadcasting construction for component template " << mName << " in object template " 
        << mObjectTemplate.getName();

	mObjectTemplate.getReplicaManager().Reference( this );
}

bool ComponentTemplate::queryBroadcastDestruction()
{
    return ( ( ( mMode == SERVER && mNetworkingType == REMOTE ) || ( mMode == CLIENT &&
        mNetworkingType == REMOTE && mSource == CLIENT ) ) && !mLocalOverride );
}

void ComponentTemplate::broadcastDestruction()
{
    OLOGD << "Broadcasting destruction for component template " << mName << " in object template " 
        << mObjectTemplate.getName();

    mBroadcastingDestruction = true;
    Replica3::BroadcastDestruction();
    mObjectTemplate.getReplicaManager().Dereference( this );
    mBroadcastingDestruction = false;
}

void ComponentTemplate::WriteAllocationID( RakNet::Connection_RM3* pConnection,
    RakNet::BitStream* pAllocationIdBitstream ) const
{
    // Write component replica type, component type and object network id.
    pAllocationIdBitstream->Write<ReplicaType>( REPLICATYPE_COMPONENTTEMPLATE );
    pAllocationIdBitstream->Write<ComponentType>( getType() );
    pAllocationIdBitstream->Write<RakNet::RakString>( RakNet::RakString( mName.c_str() ) );
    pAllocationIdBitstream->Write<RakNet::NetworkID>( mObjectTemplate.GetNetworkID() );
}

void ComponentTemplate::DeallocReplica( RakNet::Connection_RM3* pSourceConnection )
{
    // ComponentTemplate is already destroyed so no permission checking is needed, act as server.
    if( !mBroadcastingDestruction )
    {
        try
        {
            mObjectTemplate.destroyComponentTemplate( getName(), ComponentTemplate::getServerGUID() );
        }
        catch ( Exception e )
        {
            // TODO: Is this bad? Just ignoring the error for now..
            OLOGW << "Failed to deallocate component template from replica system: " << e.what();
            //DivAssert( 0, e.what() );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace ObjectSystem
} // Namespace Diversia