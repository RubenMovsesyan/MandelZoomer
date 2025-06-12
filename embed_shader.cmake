# Read the shader file
file(READ ${INPUT_FILE} SHADER_CONTENT)

# Escape quotes and backslashes
string(REPLACE "\\" "\\\\" SHADER_CONTENT "${SHADER_CONTENT}")
string(REPLACE "\"" "\\\"" SHADER_CONTENT "${SHADER_CONTENT}")
string(REPLACE "\n" "\\n\"\n\"" SHADER_CONTENT "${SHADER_CONTENT}")
