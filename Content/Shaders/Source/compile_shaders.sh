# Requires shadercross CLI installed from SDL_shadercross

echo "Compiling HLSL Shaders"

# Compile all hlsl shaders
for filename in *.vert.hlsl; do
    if [ -f "$filename" ]; then
        shadercross "$filename" -o "../Compiled/SPIRV/${filename/.hlsl/.spv}"
        shadercross "$filename" -o "../Compiled/MSL/${filename/.hlsl/.msl}"
        shadercross "$filename" -o "../Compiled/DXIL/${filename/.hlsl/.dxil}"
    fi
done

for filename in *.frag.hlsl; do
    if [ -f "$filename" ]; then
        shadercross "$filename" -o "../Compiled/SPIRV/${filename/.hlsl/.spv}"
        shadercross "$filename" -o "../Compiled/MSL/${filename/.hlsl/.msl}"
        shadercross "$filename" -o "../Compiled/DXIL/${filename/.hlsl/.dxil}"
    fi
done

for filename in *.comp.hlsl; do
    if [ -f "$filename" ]; then
        shadercross "$filename" -o "../Compiled/SPIRV/${filename/.hlsl/.spv}"
        shadercross "$filename" -o "../Compiled/MSL/${filename/.hlsl/.msl}"
        shadercross "$filename" -o "../Compiled/DXIL/${filename/.hlsl/.dxil}"
    fi
done


# echo "Compiling WGSL Shaders"

# # Compile all wgsl shaders
# for filename in *.vert.wgsl; do
#     if [ -f "$filename" ]; then
#         naga "$filename" "../Compiled/SPIRV/${filename/.wgsl/.spv}"
#         naga "$filename" "../Compiled/MSL/${filename/.wgsl/.metal}"
#         mv "../Compiled/MSL/${filename/.wgsl/.metal}" "../Compiled/MSL/${filename/.wgsl/.msl}"
#     fi
# done


# for filename in *.frag.wgsl; do
#     if [ -f "$filename" ]; then
#         naga "$filename" "../Compiled/SPIRV/${filename/.wgsl/.spv}"
#         naga "$filename" "../Compiled/MSL/${filename/.wgsl/.metal}"
#         mv "../Compiled/MSL/${filename/.wgsl/.metal}" "../Compiled/MSL/${filename/.wgsl/.msl}"
#     fi
# done


# for filename in *.comp.wgsl; do
#     if [ -f "$filename" ]; then
#         naga "$filename" "../Compiled/SPIRV/${filename/.wgsl/.spv}"
#         naga "$filename" "../Compiled/MSL/${filename/.wgsl/.metal}"
#         mv "../Compiled/MSL/${filename/.wgsl/.metal}" "../Compiled/MSL/${filename/.wgsl/.msl}"
#     fi
# done
