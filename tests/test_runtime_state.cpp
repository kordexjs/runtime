/**
 *
 *  @file test_runtime_state.cpp
 *  @author Softadastra
 *
 *  Copyright 2026, Softadastra.
 *  All rights reserved.
 *  https://github.com/softadastra/kordex-runtime
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the LICENSE file.
 *
 *  Kordex Runtime
 *
 */

#include <string_view>
#include <kordex/runtime/RuntimeState.hpp>

namespace
{
  bool expect_true(bool condition, const char *message)
  {
    if (!condition)
    {
      (void)message;
      return false;
    }

    return true;
  }

  bool test_initial_state()
  {
    kordex::runtime::RuntimeState state;

    return expect_true(
               state.value() == kordex::runtime::RuntimeLifecycleState::Created,
               "initial state should be created") &&
           expect_true(
               state.is_created(),
               "state should report created") &&
           expect_true(
               !state.is_starting(),
               "state should not report starting") &&
           expect_true(
               !state.is_running(),
               "state should not report running") &&
           expect_true(
               !state.is_stopping(),
               "state should not report stopping") &&
           expect_true(
               !state.is_stopped(),
               "state should not report stopped") &&
           expect_true(
               !state.is_failed(),
               "state should not report failed") &&
           expect_true(
               state.can_start(),
               "created state should allow start") &&
           expect_true(
               !state.can_stop(),
               "created state should not allow stop");
  }

  bool test_starting_transition()
  {
    kordex::runtime::RuntimeState state;

    const auto error = state.mark_starting();

    return expect_true(
               !error.has_error(),
               "mark_starting should succeed from created") &&
           expect_true(
               state.is_starting(),
               "state should be starting") &&
           expect_true(
               !state.can_start(),
               "starting state should not allow start") &&
           expect_true(
               state.can_stop(),
               "starting state should allow stop");
  }

  bool test_running_transition()
  {
    kordex::runtime::RuntimeState state;

    const auto starting_error = state.mark_starting();
    const auto running_error = state.mark_running();

    return expect_true(
               !starting_error.has_error(),
               "mark_starting should succeed") &&
           expect_true(
               !running_error.has_error(),
               "mark_running should succeed from starting") &&
           expect_true(
               state.is_running(),
               "state should be running") &&
           expect_true(
               !state.can_start(),
               "running state should not allow start") &&
           expect_true(
               state.can_stop(),
               "running state should allow stop");
  }

  bool test_stopping_transition_from_running()
  {
    kordex::runtime::RuntimeState state;

    const auto starting_error = state.mark_starting();
    const auto running_error = state.mark_running();
    const auto stopping_error = state.mark_stopping();

    return expect_true(
               !starting_error.has_error(),
               "mark_starting should succeed") &&
           expect_true(
               !running_error.has_error(),
               "mark_running should succeed") &&
           expect_true(
               !stopping_error.has_error(),
               "mark_stopping should succeed from running") &&
           expect_true(
               state.is_stopping(),
               "state should be stopping") &&
           expect_true(
               !state.can_start(),
               "stopping state should not allow start") &&
           expect_true(
               !state.can_stop(),
               "stopping state should not allow stop");
  }

  bool test_stopping_transition_from_starting()
  {
    kordex::runtime::RuntimeState state;

    const auto starting_error = state.mark_starting();
    const auto stopping_error = state.mark_stopping();

    return expect_true(
               !starting_error.has_error(),
               "mark_starting should succeed") &&
           expect_true(
               !stopping_error.has_error(),
               "mark_stopping should succeed from starting") &&
           expect_true(
               state.is_stopping(),
               "state should be stopping");
  }

  bool test_stopped_transition()
  {
    kordex::runtime::RuntimeState state;

    const auto starting_error = state.mark_starting();
    const auto running_error = state.mark_running();
    const auto stopping_error = state.mark_stopping();
    const auto stopped_error = state.mark_stopped();

    return expect_true(
               !starting_error.has_error(),
               "mark_starting should succeed") &&
           expect_true(
               !running_error.has_error(),
               "mark_running should succeed") &&
           expect_true(
               !stopping_error.has_error(),
               "mark_stopping should succeed") &&
           expect_true(
               !stopped_error.has_error(),
               "mark_stopped should succeed") &&
           expect_true(
               state.is_stopped(),
               "state should be stopped") &&
           expect_true(
               state.can_start(),
               "stopped state should allow restart") &&
           expect_true(
               !state.can_stop(),
               "stopped state should not allow stop");
  }

  bool test_restart_from_stopped()
  {
    kordex::runtime::RuntimeState state;

    const auto first_start = state.mark_starting();
    const auto first_stop = state.mark_stopped();
    const auto second_start = state.mark_starting();

    return expect_true(
               !first_start.has_error(),
               "first start should succeed") &&
           expect_true(
               !first_stop.has_error(),
               "mark_stopped should succeed") &&
           expect_true(
               !second_start.has_error(),
               "second start should succeed from stopped") &&
           expect_true(
               state.is_starting(),
               "state should be starting after restart");
  }

  bool test_invalid_mark_running_without_starting()
  {
    kordex::runtime::RuntimeState state;

    const auto error = state.mark_running();

    return expect_true(
               error.has_error(),
               "mark_running should fail before starting") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidState,
               "mark_running error should use InvalidState") &&
           expect_true(
               state.is_created(),
               "state should remain created");
  }

  bool test_invalid_start_while_running()
  {
    kordex::runtime::RuntimeState state;

    const auto first_start = state.mark_starting();
    const auto running = state.mark_running();
    const auto second_start = state.mark_starting();

    return expect_true(
               !first_start.has_error(),
               "first start should succeed") &&
           expect_true(
               !running.has_error(),
               "mark_running should succeed") &&
           expect_true(
               second_start.has_error(),
               "second start should fail while running") &&
           expect_true(
               second_start.code() == kordex::runtime::ErrorCode::InvalidState,
               "second start error should use InvalidState") &&
           expect_true(
               state.is_running(),
               "state should remain running");
  }

  bool test_invalid_stop_from_created()
  {
    kordex::runtime::RuntimeState state;

    const auto error = state.mark_stopping();

    return expect_true(
               error.has_error(),
               "mark_stopping should fail from created") &&
           expect_true(
               error.code() == kordex::runtime::ErrorCode::InvalidState,
               "mark_stopping error should use InvalidState") &&
           expect_true(
               state.is_created(),
               "state should remain created");
  }

  bool test_failed_state()
  {
    kordex::runtime::RuntimeState state;

    state.mark_failed();

    const auto stopped_error = state.mark_stopped();

    return expect_true(
               state.is_failed(),
               "state should be failed") &&
           expect_true(
               !state.can_start(),
               "failed state should not allow start") &&
           expect_true(
               !state.can_stop(),
               "failed state should not allow stop") &&
           expect_true(
               stopped_error.has_error(),
               "failed state should not be marked stopped directly");
  }

  bool test_reset()
  {
    kordex::runtime::RuntimeState state;

    state.mark_failed();
    state.reset();

    return expect_true(
               state.is_created(),
               "reset should return state to created") &&
           expect_true(
               state.can_start(),
               "reset state should allow start");
  }

  bool test_state_to_string()
  {
    using kordex::runtime::RuntimeLifecycleState;

    return expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeLifecycleState::Created)) == "created",
               "created should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeLifecycleState::Starting)) == "starting",
               "starting should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeLifecycleState::Running)) == "running",
               "running should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeLifecycleState::Stopping)) == "stopping",
               "stopping should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeLifecycleState::Stopped)) == "stopped",
               "stopped should convert to string") &&
           expect_true(
               std::string_view(kordex::runtime::to_string(RuntimeLifecycleState::Failed)) == "failed",
               "failed should convert to string");
  }

  bool run_tests()
  {
    return test_initial_state() &&
           test_starting_transition() &&
           test_running_transition() &&
           test_stopping_transition_from_running() &&
           test_stopping_transition_from_starting() &&
           test_stopped_transition() &&
           test_restart_from_stopped() &&
           test_invalid_mark_running_without_starting() &&
           test_invalid_start_while_running() &&
           test_invalid_stop_from_created() &&
           test_failed_state() &&
           test_reset() &&
           test_state_to_string();
  }
} // namespace

int main()
{
  return run_tests() ? 0 : 1;
}
