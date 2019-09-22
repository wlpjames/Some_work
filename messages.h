/*
  ==============================================================================

    messages.h
    Created: 21 Aug 2019 1:27:44pm
    Author:  billy


	EXAMPLE USAGE:

	To create message:

		message* cmd = new message(0, 1, "gain\0", value);

		then pass message around.

	To read:

		cmd->get_key <const char*> ();

	NOTES:

		TODO::There should be some type error checking included as it is easy to use the wrong type.

		TODO:: if possable a get_type function would be very usefull.
  ==============================================================================
*/

#pragma once
#include <iostream>
#include <string>
using namespace std;

//template <typename Type>
template <typename KT, typename VT>
struct parameters {
	KT key;
	VT value;
};

struct message {

	//IDs
	int target_id;
	int sender_id;

	template <typename t>
	struct key_struct {
		t key;
	};
	template <typename t>
	struct value_struct {
		t value;
	};

	void* key;
	void* value;

	//functions
	template <typename value_type, typename key_type>
	message(int Target_id, int Sender_id, key_type Key, value_type Value) {

		target_id = Target_id;
		sender_id = Sender_id;

		key_struct<key_type>* k = new key_struct<key_type>;
		k->key = Key;
		key = (void*)k;

		value_struct<value_type>* v = new value_struct<value_type>;
		v->value = Value;
		value = (void*)v;

		return;
	}

	//protect for exceptions here
	template <typename t> t get_key() {
		return (t)((key_struct<t>*) key)->key;
	}

	template <typename t> t get_value() {
		t val = (t)((value_struct<t>*) value)->value;
		return val;
	}

};
