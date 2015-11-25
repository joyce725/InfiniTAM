// Copyright 2014-2015 Isis Innovation Limited and the authors of InfiniTAM

#pragma once

#include <cassert>

#include "../../ORUtils/MemoryBlock.h"
#include "../Utils/ITMMath.h"

namespace ITMLib
{
  //#################### CONSTANTS ####################

  /** The maximum number of surfels that we can store in a scene. */
  const size_t MAX_SURFEL_COUNT = 10000000;/*67108864;*/ // 2^26

  //#################### TYPES ####################

  /**
   * \brief An instance of this struct represents a surfel that does not contain colour information.
   */
  struct ITMSurfel
  {
    static const CONSTPTR(bool) hasColourInformation = false;

    // Note: The ordering of the variables here matters because it affects padding - do not reorder without prior thought.

    /** The surface normal at the surfel. */
    Vector3f normal;

    /** The position of the surfel. */
    Vector3f position;

    /** The confidence counter for the surfel. */
    float confidence;

    /** The radius of the surfel. */
    float radius;

    /** A timestamp for the surfel, recording the last frame in which it was updated. */
    int timestamp;
  };

  /**
   * \brief An instance of this struct represents a surfel that contains colour information.
   */
  struct ITMSurfel_rgb
  {
    static const CONSTPTR(bool) hasColourInformation = true;

    // Note: The ordering of the variables here matters because it affects padding - do not reorder without prior thought.

    /** The RGB colour of the surfel. */
    Vector3u colour;

    /** The surface normal at the surfel. */
    Vector3f normal;

    /** The position of the surfel. */
    Vector3f position;

    /** The confidence counter for the surfel. */
    float confidence;

    /** The radius of the surfel. */
    float radius;

    /** A timestamp for the surfel, recording the last frame in which it was updated. */
    int timestamp;
  };

  /**
   * \brief TODO
   */
  template <bool hasColour> struct SurfelColourManipulator;

  /**
   * \brief TODO
   */
  template <>
  struct SurfelColourManipulator<false>
  {
    template <typename TSurfel>
    _CPU_AND_GPU_CODE_
    static Vector3u read(const TSurfel& surfel)
    {
      return Vector3u((uchar)0);
    }

    template <typename TSurfel>
    _CPU_AND_GPU_CODE_
    static void write(TSurfel& surfel, const Vector3u& colour)
    {
      // No-op
    }
  };

  /**
   * \brief TODO
   */
  template <>
  struct SurfelColourManipulator<true>
  {
    template <typename TSurfel>
    _CPU_AND_GPU_CODE_
    static Vector3u read(const TSurfel& surfel)
    {
      return surfel.colour;
    }

    template <typename TSurfel>
    _CPU_AND_GPU_CODE_
    static void write(TSurfel& surfel, const Vector3u& colour)
    {
      surfel.colour = colour;
    }
  };

  /**
   * \brief An instance of an instantiation of this class template represents a surfel-based scene.
   */
  template <typename TSurfel>
  class ITMSurfelScene
  {
    //#################### PRIVATE VARIABLES ####################
  private:
    /** The type of memory in which the scene is stored. */
    MemoryDeviceType m_memoryType;

    /** The number of surfels currently in the scene. */
    size_t m_surfelCount;

    /** The surfels in the scene. */
    ORUtils::MemoryBlock<TSurfel> *m_surfelsMB;

    //#################### CONSTRUCTORS ####################
  public:
    /**
     * \brief Constructs a surfel-based scene.
     *
     * \param memoryType  The type of memory in which to store the scene.
     */
    explicit ITMSurfelScene(MemoryDeviceType memoryType)
      : m_memoryType(memoryType),
        m_surfelCount(0),
        m_surfelsMB(new ORUtils::MemoryBlock<TSurfel>(MAX_SURFEL_COUNT, true, true))
    {}

    //#################### DESTRUCTOR ####################
  public:
    /**
     * \brief Destroys the scene.
     */
    ~ITMSurfelScene()
    {
      delete m_surfelsMB;
    }

    //#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################
  private:
    // Deliberately private and unimplemented
    ITMSurfelScene(const ITMSurfelScene&);
    ITMSurfelScene& operator=(const ITMSurfelScene&);

    //#################### PUBLIC MEMBER FUNCTIONS ####################
  public:
    /**
     * \brief TODO
     */
    TSurfel *AllocateSurfels(size_t newSurfelCount)
    {
      if(m_surfelCount + newSurfelCount > m_surfelsMB->dataSize) return NULL;
      TSurfel *newSurfels = m_surfelsMB->GetData(m_memoryType) + m_surfelCount;
      m_surfelCount += newSurfelCount;
      return newSurfels;
    }

    /**
     * \brief TODO
     */
    size_t GetSurfelCount() const
    {
      return m_surfelCount;
    }

    /**
     * \brief TODO
     */
    const ORUtils::MemoryBlock<TSurfel> *GetSurfels() const
    {
      return m_surfelsMB;
    }

    /**
     * \brief Resets the scene.
     */
    void Reset()
    {
      m_surfelCount = 0;
    }
  };
}
