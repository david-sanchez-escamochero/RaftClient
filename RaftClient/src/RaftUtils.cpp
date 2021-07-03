#include "RaftUtils.h"
#include "ClientDefs.h"


std::string RaftUtils::parse_state_to_string(StateEnum state) {
	std::string str;
	if (state == StateEnum::candidate_state)
		str = std::string(CANDIDATE_TEXT);
	else if (state == StateEnum::follower_state)
		str = std::string(FOLLOWER_TEXT);
	else if (state == StateEnum::leader_state)
		str = std::string(LEADER_TEXT);
	else
		str = std::string(UNKNOWN_TEXT);

	return str;
}


std::string RaftUtils::parse_from_socket_enum_to_text(ClientRequesTypeEnum type)
{
	std::string ret;
	switch (type) {

	case ClientRequesTypeEnum::client_write_master:
		ret = std::string(CLIENT_REQUEST_VALUE_TEXT);
		break;
	case ClientRequesTypeEnum::client_request_leader:
		ret = std::string(CLIENT_REQUEST_LEADER_TEXT);
		break;
	default:
		ret = std::string(UNKNOWN_TEXT);
	}
	return ret;
}


void RaftUtils::from_degrees_to_frame(char *can_frame, int count)
{
	switch(count){
	case 4:
		can_frame[17] = '0';
		can_frame[19] = '4';
		break;
	case 5:
		can_frame[17] = '0';
		can_frame[19] = '5';
		break;
	case 6:
		can_frame[17] = '0';
		can_frame[19] = '6';
		break;
	case 7:
		can_frame[17] = '0';
		can_frame[19] = '7';
		break;
	case 8:
		can_frame[17] = '0';
		can_frame[19] = '8';
		break;
	case 9:
		can_frame[17] = '0';
		can_frame[19] = '9';
		break;
	case 10:
		can_frame[17] = '1';
		can_frame[19] = '0';
		break;
	case 11:
		can_frame[17] = '1';
		can_frame[19] = '1';
		break;
	case 12:
		can_frame[17] = '1';
		can_frame[19] = '2';
		break;
	case 13:
		can_frame[17] = '1';
		can_frame[19] = '3';
		break;
	case 14:
		can_frame[17] = '1';
		can_frame[19] = '4';
		break;
	case 15:
		can_frame[17] = '1';
		can_frame[19] = '5';
		break;
	case 16:
		can_frame[17] = '1';
		can_frame[19] = '6';
		break;
	case 17:
		can_frame[17] = '1';
		can_frame[19] = '7';
		break;
	case 18:
		can_frame[17] = '1';
		can_frame[19] = '8';
		break;
	case 19:
		can_frame[17] = '1';
		can_frame[19] = '9';
		break;
	case 20:
		can_frame[17] = '2';
		can_frame[19] = '0';
		break;
	case 21:
		can_frame[17] = '2';
		can_frame[19] = '1';
		break;
	case 22:
		can_frame[17] = '2';
		can_frame[19] = '2';
		break;
	case 23:
		can_frame[17] = '2';
		can_frame[19] = '3';
		break;
	case 24:
		can_frame[17] = '2';
		can_frame[19] = '4';
		break;
	case 25:
		can_frame[17] = '2';
		can_frame[19] = '5';
		break;
	case 26:
		can_frame[17] = '2';
		can_frame[19] = '6';
		break;

	}
}

//
//std::string RaftUtils::parse_from_rpc_enum_to_text(RPCTypeEnum type)
//{
//	std::string ret;
//	switch (type) {
//	case RPCTypeEnum::rpc_append_entry:
//		ret = std::string(APPEND_ENTRY_TEXT);
//		break;
//	case RPCTypeEnum::rpc_append_heart_beat:
//		ret = std::string(HEART_BEAT_TEXT);
//		break;
//	case RPCTypeEnum::rpc_append_request_vote:
//		ret = std::string(REQUEST_VOTE_TEXT);
//		break;
//	default:
//		ret = std::string(UNKNOWN_TEXT);
//	}
//	return ret;
//}

