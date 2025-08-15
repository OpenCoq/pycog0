#!/usr/bin/env python3
"""
Validation script for the generate-next-steps workflow.
This script tests the parsing logic and JSON generation locally.
"""

import re
import json
import sys
import os
import base64
from pathlib import Path

def validate_genesis_document():
    """Validate the structure and content of AGENT-ZERO-GENESIS.md."""
    genesis_file = Path('AGENT-ZERO-GENESIS.md')
    
    if not genesis_file.exists():
        print("❌ AGENT-ZERO-GENESIS.md not found")
        return False
    
    try:
        content = genesis_file.read_text(encoding='utf-8')
    except Exception as e:
        print(f"❌ Failed to read AGENT-ZERO-GENESIS.md: {e}")
        return False
    
    # Check for control characters
    control_chars = []
    for i, char in enumerate(content):
        if ord(char) < 32 and char not in '\n\r\t':
            control_chars.append((i, repr(char), ord(char)))
    
    if control_chars:
        print(f"⚠️  Found {len(control_chars)} control characters")
        for pos, char_repr, char_ord in control_chars[:5]:
            print(f"   Position {pos}: {char_repr} (ord: {char_ord})")
    else:
        print("✅ No problematic control characters found")
    
    # Check structure
    phase_headers = re.findall(r'^### Phase \d+:', content, re.MULTILINE)
    task_lines = re.findall(r'^- \[ \] \*\*[A-Z]+-[A-Z]+-\d+\*\*:', content, re.MULTILINE)
    
    print(f"✅ Found {len(phase_headers)} phase headers")
    print(f"✅ Found {len(task_lines)} task lines")
    
    if len(phase_headers) == 0:
        print("❌ No phase headers found")
        return False
    if len(task_lines) == 0:
        print("❌ No task lines found")
        return False
    
    return True

def parse_genesis_document():
    """Parse AGENT-ZERO-GENESIS.md and extract task information."""
    try:
        with open('AGENT-ZERO-GENESIS.md', 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"❌ Failed to read file: {e}")
        return {}
    
    # Clean control characters
    content = re.sub(r'[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]', '', content)
    
    phases = {}
    current_phase = None
    
    lines = content.split('\n')
    
    for line in lines:
        line = line.strip()
        if not line:
            continue
            
        # Match phase headers
        phase_match = re.match(r'^### Phase (\d+): (.+)$', line)
        if phase_match:
            phase_num = phase_match.group(1)
            phase_name = phase_match.group(2)
            current_phase = phase_num
            phases[current_phase] = {
                'name': phase_name,
                'tasks': []
            }
            continue
        
        # Match task lines
        task_match = re.match(r'^- \[ \] \*\*([A-Z]+-[A-Z]+-\d+)\*\*: (.+)$', line)
        if task_match and current_phase:
            task_id = task_match.group(1)
            task_description = task_match.group(2)
            
            task_parts = task_id.split('-')
            if len(task_parts) >= 3:
                phases[current_phase]['tasks'].append({
                    'id': task_id,
                    'title': task_description,
                    'category': task_parts[1],
                    'number': task_parts[2],
                    'phase': current_phase,
                    'phase_name': phases[current_phase]['name']
                })
    
    return phases

def test_json_generation(phases, test_phase='1'):
    """Test JSON generation and encoding for the specified phase."""
    if test_phase not in phases:
        print(f"❌ Phase {test_phase} not found")
        return False
    
    # Generate test issues
    issues = []
    for task in phases[test_phase]['tasks']:
        issue = {
            'title': f"{task['id']}: {task['title']}",
            'body': f"""## 🎯 Task Description

{task['title']}

**Phase**: {task['phase']} - {task['phase_name']}
**Category**: {task['category']}
**Task ID**: `{task['id']}`

## Implementation Guidelines

This task requires implementation following OpenCog patterns.

## Acceptance Criteria

- [ ] Implementation completed
- [ ] Tests pass
- [ ] Documentation updated

---

*Auto-generated from AGENT-ZERO-GENESIS.md*
""",
            'labels': ['enhancement', f"phase-{task['phase']}", task['category'].lower()],
            'priority': 'high'
        }
        issues.append(issue)
    
    # Test JSON generation
    try:
        issues_json = json.dumps(issues, indent=2, ensure_ascii=False, separators=(',', ': '))
        issues_json = re.sub(r'[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]', '', issues_json)
        
        print(f"✅ Generated JSON for {len(issues)} issues ({len(issues_json)} chars)")
    except Exception as e:
        print(f"❌ JSON generation failed: {e}")
        return False
    
    # Test base64 encoding
    try:
        encoded = base64.b64encode(issues_json.encode('utf-8')).decode('ascii')
        print(f"✅ Base64 encoded ({len(encoded)} chars)")
    except Exception as e:
        print(f"❌ Base64 encoding failed: {e}")
        return False
    
    # Test decoding
    try:
        decoded = base64.b64decode(encoded.encode('ascii')).decode('utf-8')
        parsed = json.loads(decoded)
        print(f"✅ Successfully decoded and parsed {len(parsed)} issues")
    except Exception as e:
        print(f"❌ Decoding failed: {e}")
        return False
    
    # Verify data integrity
    if len(parsed) != len(issues):
        print(f"❌ Data integrity failed: {len(issues)} != {len(parsed)}")
        return False
    
    print("✅ Data integrity verified")
    return True

def main():
    """Run all validation tests."""
    print("🔍 Validating generate-next-steps workflow components...")
    print("=" * 60)
    
    # Test 1: Validate file structure
    print("1. Validating AGENT-ZERO-GENESIS.md structure...")
    if not validate_genesis_document():
        print("❌ File validation failed")
        return False
    print("✅ File validation passed")
    print()
    
    # Test 2: Parse document
    print("2. Testing document parsing...")
    phases = parse_genesis_document()
    if not phases:
        print("❌ Document parsing failed")
        return False
    
    total_tasks = sum(len(phase['tasks']) for phase in phases.values())
    print(f"✅ Parsed {len(phases)} phases with {total_tasks} total tasks")
    print()
    
    # Test 3: JSON generation
    print("3. Testing JSON generation and encoding...")
    if not test_json_generation(phases):
        print("❌ JSON generation test failed")
        return False
    print("✅ JSON generation test passed")
    print()
    
    print("🎉 All validation tests passed!")
    print("The generate-next-steps workflow should work correctly.")
    return True

if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)