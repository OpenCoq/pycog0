/*
 * src/CognitiveLoop.cpp
 *
 * Copyright (C) 2024 OpenCog Foundation
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Cognitive Loop Implementation
 * Implements perception-action-reflection cycle with AtomSpace integration
 * Part of the AGENT-ZERO-GENESIS project
 */

#include <sstream>
#include <thread>
#include <chrono>

#include <opencog/atoms/atom_types/types.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/truthvalue/SimpleTruthValue.h>

#include "opencog/agentzero/CognitiveLoop.h"
#include "opencog/agentzero/AgentZeroCore.h"

using namespace opencog;
using namespace opencog::agentzero;

CognitiveLoop::CognitiveLoop(AgentZeroCore* agent_core, AtomSpacePtr atomspace)
    : _agent_core(agent_core)
    , _atomspace(atomspace)
    , _running(false)
    , _paused(false)
    , _cycle_interval(std::chrono::milliseconds(1000)) // Default 1 second
    , _cycle_count(0)
    , _last_cycle_duration_ms(0)
    , _perception_context(Handle::UNDEFINED)
    , _planning_context(Handle::UNDEFINED)
    , _action_context(Handle::UNDEFINED)
    , _reflection_context(Handle::UNDEFINED)
    , _enable_perception(true)
    , _enable_planning(true)
    , _enable_action(true)
    , _enable_reflection(true)
{
    logger().info() << "[CognitiveLoop] Constructor: Initializing cognitive loop";
    
    // Create context atoms for each cognitive phase
    std::string agent_name = _agent_core->getAgentName();
    _perception_context = _atomspace->add_node(CONCEPT_NODE, agent_name + "_Perception");
    _planning_context = _atomspace->add_node(CONCEPT_NODE, agent_name + "_Planning");
    _action_context = _atomspace->add_node(CONCEPT_NODE, agent_name + "_Action");
    _reflection_context = _atomspace->add_node(CONCEPT_NODE, agent_name + "_Reflection");
    
    logger().debug() << "[CognitiveLoop] Context atoms created";
}

CognitiveLoop::~CognitiveLoop()
{
    logger().info() << "[CognitiveLoop] Destructor: Cleaning up cognitive loop";
    
    // Ensure loop is stopped before destruction
    if (_running.load()) {
        stop();
    }
}

bool CognitiveLoop::start()
{
    if (_running.load()) {
        logger().warn() << "[CognitiveLoop] Already running";
        return true;
    }
    
    logger().info() << "[CognitiveLoop] Starting cognitive loop";
    
    try {
        _running = true;
        _paused = false;
        
        // Start the main loop in a separate thread
        _loop_thread = std::make_unique<std::thread>(&CognitiveLoop::runMainLoop, this);
        
        logger().info() << "[CognitiveLoop] Cognitive loop started successfully";
        return true;
        
    } catch (const std::exception& e) {
        logger().error() << "[CognitiveLoop] Failed to start: " << e.what();
        _running = false;
        return false;
    }
}

bool CognitiveLoop::stop()
{
    if (!_running.load()) {
        logger().warn() << "[CognitiveLoop] Not running";
        return true;
    }
    
    logger().info() << "[CognitiveLoop] Stopping cognitive loop";
    
    _running = false;
    _paused = false;
    
    // Wait for thread to complete
    if (_loop_thread && _loop_thread->joinable()) {
        _loop_thread->join();
        _loop_thread.reset();
    }
    
    logger().info() << "[CognitiveLoop] Cognitive loop stopped successfully";
    return true;
}

bool CognitiveLoop::pause()
{
    if (!_running.load()) {
        logger().warn() << "[CognitiveLoop] Cannot pause: not running";
        return false;
    }
    
    if (_paused.load()) {
        logger().warn() << "[CognitiveLoop] Already paused";
        return true;
    }
    
    logger().info() << "[CognitiveLoop] Pausing cognitive loop";
    _paused = true;
    return true;
}

bool CognitiveLoop::resume()
{
    if (!_running.load()) {
        logger().warn() << "[CognitiveLoop] Cannot resume: not running";
        return false;
    }
    
    if (!_paused.load()) {
        logger().warn() << "[CognitiveLoop] Not paused";
        return true;
    }
    
    logger().info() << "[CognitiveLoop] Resuming cognitive loop";
    _paused = false;
    return true;
}

bool CognitiveLoop::executeSingleCycle()
{
    logger().debug() << "[CognitiveLoop] Executing single cognitive cycle";
    
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        bool cycle_success = true;
        
        // Execute each cognitive phase if enabled
        if (_enable_perception) {
            cycle_success &= executePerceptionPhase();
        }
        
        if (_enable_planning) {
            cycle_success &= executePlanningPhase();
        }
        
        if (_enable_action) {
            cycle_success &= executeActionPhase();
        }
        
        if (_enable_reflection) {
            cycle_success &= executeReflectionPhase();
        }
        
        // Update cycle metrics
        updateCycleMetrics(start_time);
        
        // Increment cycle count
        _cycle_count++;
        
        logger().debug() << "[CognitiveLoop] Cycle " << _cycle_count << " completed in " 
                        << _last_cycle_duration_ms << "ms";
        
        return cycle_success;
        
    } catch (const std::exception& e) {
        handleLoopException(e);
        return false;
    }
}

void CognitiveLoop::configurePhases(bool perception, bool planning, bool action, bool reflection)
{
    logger().info() << "[CognitiveLoop] Configuring phases: perception=" << perception 
                   << ", planning=" << planning << ", action=" << action 
                   << ", reflection=" << reflection;
    
    _enable_perception = perception;
    _enable_planning = planning;
    _enable_action = action;
    _enable_reflection = reflection;
}

std::string CognitiveLoop::getStatusInfo() const
{
    std::ostringstream status;
    status << "{";
    status << "\"running\":" << (_running.load() ? "true" : "false") << ",";
    status << "\"paused\":" << (_paused.load() ? "true" : "false") << ",";
    status << "\"cycle_count\":" << _cycle_count.load() << ",";
    status << "\"last_cycle_duration_ms\":" << _last_cycle_duration_ms.load() << ",";
    status << "\"cycle_interval_ms\":" << _cycle_interval.count() << ",";
    status << "\"perception_enabled\":" << (_enable_perception ? "true" : "false") << ",";
    status << "\"planning_enabled\":" << (_enable_planning ? "true" : "false") << ",";
    status << "\"action_enabled\":" << (_enable_action ? "true" : "false") << ",";
    status << "\"reflection_enabled\":" << (_enable_reflection ? "true" : "false");
    status << "}";
    return status.str();
}

// Private implementation methods

void CognitiveLoop::runMainLoop()
{
    logger().info() << "[CognitiveLoop] Main loop thread started";
    
    while (_running.load()) {
        try {
            // Check if paused
            if (_paused.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            // Execute one cognitive cycle
            executeSingleCycle();
            
            // Sleep for the specified interval
            std::this_thread::sleep_for(_cycle_interval);
            
        } catch (const std::exception& e) {
            handleLoopException(e);
            
            // Brief pause before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    logger().info() << "[CognitiveLoop] Main loop thread terminated";
}

bool CognitiveLoop::executePerceptionPhase()
{
    logger().debug() << "[CognitiveLoop] Executing perception phase";
    
    try {
        // Update perception context with current timestamp
        TruthValuePtr perception_tv = SimpleTruthValue::createTV(0.8, 0.9);
        _perception_context->setTruthValue(perception_tv);
        
        // Basic perception processing - in a full implementation this would:
        // - Process sensory inputs
        // - Update world model in AtomSpace
        // - Detect changes and events
        // - Update attention allocation
        
        // For now, just record that perception occurred
        HandleSeq perception_link;
        perception_link.push_back(_agent_core->getAgentSelfAtom());
        perception_link.push_back(_perception_context);
        _atomspace->add_link(EVALUATION_LINK, perception_link);
        
        return true;
        
    } catch (const std::exception& e) {
        logger().error() << "[CognitiveLoop] Perception phase error: " << e.what();
        return false;
    }
}

bool CognitiveLoop::executePlanningPhase()
{
    logger().debug() << "[CognitiveLoop] Executing planning phase";
    
    try {
        // Update planning context
        TruthValuePtr planning_tv = SimpleTruthValue::createTV(0.7, 0.8);
        _planning_context->setTruthValue(planning_tv);
        
        // Basic planning processing - in a full implementation this would:
        // - Analyze current goals
        // - Generate action plans
        // - Evaluate plan feasibility
        // - Select optimal actions
        
        // Delegate to TaskManager if available
        if (_agent_core->getTaskManager()) {
            return _agent_core->getTaskManager()->processTaskManagement();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logger().error() << "[CognitiveLoop] Planning phase error: " << e.what();
        return false;
    }
}

bool CognitiveLoop::executeActionPhase()
{
    logger().debug() << "[CognitiveLoop] Executing action phase";
    
    try {
        // Update action context
        TruthValuePtr action_tv = SimpleTruthValue::createTV(0.6, 0.7);
        _action_context->setTruthValue(action_tv);
        
        // Basic action processing - in a full implementation this would:
        // - Execute planned actions
        // - Monitor action outcomes
        // - Handle action failures
        // - Update environment state
        
        // For now, just record that action phase occurred
        HandleSeq action_link;
        action_link.push_back(_agent_core->getAgentSelfAtom());
        action_link.push_back(_action_context);
        _atomspace->add_link(EVALUATION_LINK, action_link);
        
        return true;
        
    } catch (const std::exception& e) {
        logger().error() << "[CognitiveLoop] Action phase error: " << e.what();
        return false;
    }
}

bool CognitiveLoop::executeReflectionPhase()
{
    logger().debug() << "[CognitiveLoop] Executing reflection phase";
    
    try {
        // Update reflection context
        TruthValuePtr reflection_tv = SimpleTruthValue::createTV(0.5, 0.6);
        _reflection_context->setTruthValue(reflection_tv);
        
        // Basic reflection processing - in a full implementation this would:
        // - Analyze action outcomes
        // - Update knowledge base
        // - Learn from experience
        // - Adjust future behavior
        
        // Delegate to KnowledgeIntegrator if available
        if (_agent_core->getKnowledgeIntegrator()) {
            return _agent_core->getKnowledgeIntegrator()->processKnowledgeIntegration();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logger().error() << "[CognitiveLoop] Reflection phase error: " << e.what();
        return false;
    }
}

void CognitiveLoop::updateCycleMetrics(const std::chrono::steady_clock::time_point& start_time)
{
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    _last_cycle_duration_ms = duration.count();
}

void CognitiveLoop::handleLoopException(const std::exception& e)
{
    logger().error() << "[CognitiveLoop] Exception in main loop: " << e.what();
    
    // In a production system, might want to:
    // - Record error statistics
    // - Attempt recovery actions
    // - Notify monitoring systems
    // - Adjust loop parameters
}