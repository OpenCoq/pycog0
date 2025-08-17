/*
 * opencog/agentzero/AgentZeroCore.h
 *
 * Copyright (C) 2024 OpenCog Foundation
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Agent-Zero Core Orchestration Engine
 * Main cognitive architecture integration with OpenCog
 * Part of the AGENT-ZERO-GENESIS project
 */

#ifndef _OPENCOG_AGENTZERO_CORE_H
#define _OPENCOG_AGENTZERO_CORE_H

#include <memory>
#include <string>
#include <atomic>

#include <opencog/cogserver/server/Module.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Handle.h>
#include <opencog/util/Logger.h>

namespace opencog {
namespace agentzero {

// Forward declarations
class CognitiveLoop;
class TaskManager;
class KnowledgeIntegrator;

/**
 * AgentZeroCore - Main orchestration engine for Agent-Zero cognitive architecture
 *
 * This class provides the core integration between Agent-Zero and OpenCog,
 * implementing the main cognitive loop and coordination between components.
 * It inherits from OpenCog's Module class to integrate with CogServer.
 *
 * Key Features:
 * - CogServer module integration for network access
 * - AtomSpace-based state representation
 * - Cognitive loop coordination
 * - Goal and task management
 * - Knowledge integration and reasoning
 */
class AgentZeroCore : public Module
{
private:
    // Core components
    std::unique_ptr<CognitiveLoop> _cognitive_loop;
    std::unique_ptr<TaskManager> _task_manager;
    std::unique_ptr<KnowledgeIntegrator> _knowledge_integrator;
    
    // AtomSpace for state representation
    AtomSpacePtr _atomspace;
    
    // Agent state
    std::atomic<bool> _running;
    std::atomic<bool> _initialized;
    std::string _agent_name;
    
    // Core atom handles for agent state
    Handle _agent_self_atom;
    Handle _current_goal_atom;
    Handle _working_memory_atom;
    
    // Configuration
    bool _enable_cognitive_loop;
    bool _enable_goal_processing;
    bool _enable_knowledge_integration;
    
    // Internal methods
    void initializeAtomSpace();
    void createAgentSelfRepresentation();
    void setupCoreAtoms();
    void startCognitiveLoop();
    void stopCognitiveLoop();

public:
    /**
     * Constructor - Creates AgentZeroCore module
     * @param cogserver Reference to the CogServer instance
     * @param agent_name Name identifier for this agent instance
     */
    AgentZeroCore(CogServer& cogserver, const std::string& agent_name = "AgentZero");
    
    /**
     * Destructor - Cleans up resources and stops processing
     */
    virtual ~AgentZeroCore();
    
    // Module interface implementation
    virtual void init() override;
    virtual bool config(const char* config_string) override;
    virtual const char* id();
    
    // Core agent operations
    /**
     * Start the agent's cognitive processing
     * @return true if successfully started, false otherwise
     */
    bool start();
    
    /**
     * Stop the agent's cognitive processing
     * @return true if successfully stopped, false otherwise
     */
    bool stop();
    
    /**
     * Check if agent is currently running
     * @return true if agent is active, false otherwise
     */
    bool isRunning() const { return _running.load(); }
    
    /**
     * Check if agent is properly initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return _initialized.load(); }
    
    // AtomSpace integration
    /**
     * Get the agent's AtomSpace instance
     * @return shared pointer to the AtomSpace
     */
    AtomSpacePtr getAtomSpace() const { return _atomspace; }
    
    /**
     * Get the agent's self-representation atom
     * @return Handle to the agent's self atom
     */
    Handle getAgentSelfAtom() const { return _agent_self_atom; }
    
    /**
     * Get the current goal atom
     * @return Handle to the current goal atom
     */
    Handle getCurrentGoal() const { return _current_goal_atom; }
    
    /**
     * Set a new goal for the agent
     * @param goal_atom Handle to the goal atom
     * @return true if goal was set successfully
     */
    bool setGoal(const Handle& goal_atom);
    
    // Component access
    /**
     * Get the cognitive loop component
     * @return pointer to CognitiveLoop instance
     */
    CognitiveLoop* getCognitiveLoop() const { return _cognitive_loop.get(); }
    
    /**
     * Get the task manager component
     * @return pointer to TaskManager instance
     */
    TaskManager* getTaskManager() const { return _task_manager.get(); }
    
    /**
     * Get the knowledge integrator component
     * @return pointer to KnowledgeIntegrator instance
     */
    KnowledgeIntegrator* getKnowledgeIntegrator() const { return _knowledge_integrator.get(); }
    
    // Agent information
    /**
     * Get the agent's name
     * @return agent name string
     */
    const std::string& getAgentName() const { return _agent_name; }
    
    /**
     * Get agent status information
     * @return JSON string with status details
     */
    std::string getStatusInfo() const;
    
    /**
     * Process one step of the cognitive cycle
     * Called by the cognitive loop or manually for debugging
     * @return true if step completed successfully
     */
    bool processCognitiveStep();
};

} // namespace agentzero
} // namespace opencog

#endif // _OPENCOG_AGENTZERO_CORE_H