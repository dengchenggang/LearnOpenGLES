package com.dcg.unity.bridge.command;

import androidx.annotation.NonNull;

import com.dcg.utils.Debug;
import com.google.gson.TypeAdapter;
import com.google.gson.stream.JsonReader;
import com.google.gson.stream.JsonToken;
import com.google.gson.stream.JsonWriter;

import java.io.IOException;

public class Command {
    private static final String TAG = "Command::";
    private static final String PROTOCOL_TAG = "protocol";
    private static final String METHOD_TAG = "method";
    private static final String ARGS_TAG = "args";

    public final String protocol;
    public final String method;
    public final String args;

    public Command(@NonNull final String protocol, @NonNull final String method, final String args) {
        this.protocol = protocol;
        this.method = method;
        this.args = args;
    }

    @NonNull
    @Override
    public String toString() {
        final CommandAdapter commandAdapter = new CommandAdapter();
        return commandAdapter.toJson(this);
    }

    public static Command parse(@NonNull final String str) {
        final CommandAdapter adapter = new CommandAdapter();
        try {
            return adapter.fromJson(str);
        } catch (IOException e) {
            Debug.logI(TAG + "parse: str=" + str + ", e=" + e);
            return null;
        }

    }

    private static class CommandAdapter extends TypeAdapter<Command> {
        @Override
        public void write(JsonWriter jsonWriter, Command command) throws IOException {
            jsonWriter.beginObject();
            jsonWriter.name(PROTOCOL_TAG).value(command.protocol)
                    .name(METHOD_TAG).value(command.method)
                    .name(ARGS_TAG).value(command.args);
            jsonWriter.endObject();
        }

        @Override
        public Command read(JsonReader jsonReader) throws IOException {
            String protocol = null;
            String method = null;
            String args = null;
            int flags = 0;
            jsonReader.beginObject();
            while (jsonReader.hasNext()) {
                final String name = jsonReader.nextName();
                if (jsonReader.peek() == JsonToken.NULL) {
                    jsonReader.nextNull();
                    continue;
                }
                switch (name) {
                    case PROTOCOL_TAG:
                        protocol = jsonReader.nextString();
                        break;
                    case METHOD_TAG:
                        method = jsonReader.nextString();
                        break;
                    case ARGS_TAG:
                        args = jsonReader.nextString();
                        break;
                    default:
                        break;
                }
            }
            return (protocol == null) || (method == null) ? null : new Command(protocol, method, args);
        }
    }
}
