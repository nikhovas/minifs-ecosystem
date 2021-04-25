void answer_ok(int descr, int content_length, const void* contents);
void answer_created(int descr);
void answer_ok_only(int descr);

void answer_not_found(int descr);
void answer_request_entity_is_too_large(int descr);
void answer_bad_request(int descr);
void answer_internal_server_error(int descr);
void answer_internal_server_error_with_data(int descr, int content_length, const void* contents);
void answer_method_not_allowed(int descr);
void answer_payload_too_large(int descr);