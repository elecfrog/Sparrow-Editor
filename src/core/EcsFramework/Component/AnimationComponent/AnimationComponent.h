//
// Created by Shawwy on 2/24/2023.
//



#ifndef SPARROW_ANIMATIONCOMPONENT_H
#define SPARROW_ANIMATIONCOMPONENT_H

#include "../ComponentI.h"
#include "./Model/Animation/Skeleton.h"
#include <vector>
#include "glm/glm.hpp"
//#include "Animation.h"
namespace SPW {

    enum class State {started,stopped};
    using ClipTransform = std::vector<std::vector<glm::mat4>>;
    using FlattenTransform = std::vector<glm::mat4>;

    struct VerMapBone
    {
        //Vertex -> <BoneID> -> (stores Index of finalBoneMatrix)
        std::vector<uint32_t> boneID;
        //Weight -> <double) -> (stores corresponding weight; (boneID[0] - Weight[0]) )
        std::vector<float> weight;
    };

    struct AnimationClipTransform
    {
        //TODO: 二维降成一维数组，
        // 如何找到第n个骨骼的第i和i+1帧？（ a * frameCount + i)
        ClipTransform finalKeyframeMatrices;
        FlattenTransform flattenTransform;
        float frameCount;
        std::string animName;
    };

    //For all vertices
    struct VertexBoneMap
    {
        //Start index 只负责记录weights数组
        std::vector<uint32_t> startIndex;
        std::vector<uint32_t> size;
        std::vector<float> weights;
        std::vector<uint32_t> boneID;
    };

    class AnimationComponent : ComponentI
    {
    public:
        //Constructor
        AnimationComponent();

        //Variables needed
        State state = State::stopped;
        bool bInitialized = false;
        bool bLoaded = false;
        float playSpeed = 1.0f;
        float currentTime;

        std::string incomingAnimName;

        std::weak_ptr<AnimationClip> currentAnimation;

        std::shared_ptr<Skeleton> skeleton;

        std::vector<glm::mat4> finalBoneMatrices;

        std::vector<float*> finalBoneArray;

        std::vector<VerMapBone> verMapBone;

        std::vector<AnimationClipTransform> finalKeyMatricesAllClips;
        VertexBoneMap vertexBoneMap;
        int indices [2];
        float frameWeights[2];
    };


}

#endif //SPARROW_ANIMATIONCOMPONENT_H


