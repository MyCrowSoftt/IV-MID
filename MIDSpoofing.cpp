QWORD qwJoiningMachineID = 0;
DWORD dwJoiningIP;

Detour<HRESULT> CXnIp_SecRegLookupDetour;
HRESULT CXnIp_SecRegLookupHook(DWORD r3, DWORD r4, DWORD r5, DWORD r6) {

    if (r5) {
        QWORD MachineID = _byteswap_uint64(*(QWORD*)(r5 + 0x18));

        qwJoiningMachineID = MachineID;
        dwJoiningIP = ((XNADDR*)r5)->ina.S_un.S_addr;
    }

    return  CXnIp_SecRegLookupDetour.CallOriginal(r3, r4, r5, r6);
}

Detour<void> CNetwork_PeerHasJoinedDetour;
void CNetwork_PeerHasJoined(DWORD* pPeer) {

    if (pPeer && Invoker::Invoke<BOOL>(0x826C4D78) /*CNetwork::IsHost*/) {
        // Read into the peer information and get the XNADDR.
        DWORD dwJoiningPlayerInfo = Invoker::Invoke<DWORD>(0x827087D8, pPeer);
        XNADDR TheirXNADDR = (*(XNADDR*)(dwJoiningPlayerInfo));

        // Grab the machine ID 
        QWORD MachineID = 0;
        XNetXnAddrToMachineId(&TheirXNADDR, &MachineID);

        // Compare the IP from the XNADDR from the game to the one we cached from XAM.
        if (TheirXNADDR.ina.S_un.S_addr == dwJoiningIP && MachineID != qwJoiningMachineID) {
            DbgPrint("Prevented player %s from joining due to spoofed machine ID (Real: %016I64x | Fake: %016I64x)\n", (char*)(dwJoiningPlayerInfo + 0x4C), qwJoiningMachineID, MachineID);

            // CNetwork::Kick and pass in a pointer to the GamerID
            Invoker::Invoke<void>(0x826CA1A8, (QWORD*)(dwJoiningPlayerInfo + 0x30));
        }

        dwJoiningIP = 0;
        qwJoiningMachineID = 0;
    }

    return CNetwork_PeerHasJoinedDetour.CallOriginal(pPeer);
}

void SetupMIDHooks() {
	CXnIp_SecRegLookupDetour.SetupDetour(Devkit ? 0x81871F90 : 0x8174A340, CXnIp_SecRegLookupHook);
	CNetwork_PeerHasJoinedDetour.SetupDetour(0x826C4EC0, CNetwork_PeerHasJoined);
}